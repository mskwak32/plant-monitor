Sync selected markdown files from the plant-monitor project to Notion.

## Targets

- Sync `system_architecture.md` to page `3386e29b-97d5-814b-96f0-e5e4515e58c7`.
- Sync `주차별_맥락/{N}주차_맥락.md` under parent `3386e29b-97d5-80dd-8546-f9877e9f82f3`.
- Sync weekly files only when they contain `**{N}주차 완료**`.

## State

- Read `{project_root}/.agent/notion_sync_state.json`.
- Use an empty state if the file does not exist.
- Store `mtime`, `content_hash`, and `notion_page_id` for each synced file.
- Compute `content_hash` as SHA-256 of the raw source file bytes.
- Use `mtime` only as a fast change check.
- Call Notion only when the current raw-file `content_hash` differs from the saved `content_hash`.
- If `mtime` changed but `content_hash` is unchanged, skip Notion and update only the saved `mtime`.
- For legacy state entries without `content_hash`, compute and save `content_hash` during the next run. If their `mtime` is already unchanged, initialize the missing `content_hash` without calling Notion.

## Preprocess

- Remove the body h1 and use it only as the Notion title.
- Remove the entire `## 현재 진행 상황` section.
- Convert ASCII flowcharts to Mermaid when the structure is clear and the result is reliable.
- Keep the original ASCII art in the Notion-synced markdown when Mermaid conversion is unclear.
- When Mermaid conversion is unclear, also generate an SVG version and save it in the same context folder as the source markdown.
- Check that the markdown will not break badly in Notion.

## Tools

- Use `mcp__codex_apps__notion_fetch` for page lookup.
- Use `mcp__codex_apps__notion_notion_create_pages` for new pages.
- Use `mcp__codex_apps__notion_notion_update_page` for existing pages.

## Run

1. Read the state file.
2. Check `system_architecture.md`.
3. Skip it if `mtime` is unchanged and `content_hash` exists.
4. Compute SHA-256 `content_hash` from the raw source file if `mtime` changed or `content_hash` is missing.
5. Add it to the candidate list when `content_hash` changed, or to the skip list when `content_hash` is unchanged.
6. Iterate all `주차별_맥락/*주차_맥락.md` files.
7. Skip files without `**{N}주차 완료**`.
8. Add files whose `mtime` is unchanged and `content_hash` exists to the skip list.
9. Compute SHA-256 `content_hash` from the raw source file for changed weekly files, or legacy weekly files missing `content_hash`.
10. Add changed weekly files to the candidate list, unchanged files to the skip list, and legacy unchanged files missing `content_hash` to the state-only update list.
11. Print the candidate list before any Notion create/update call.
12. Print the skip list and state-only update list for visibility.
13. Ask the user whether to proceed with the candidate sync.
14. Stop without calling Notion or writing the state file unless the user explicitly approves.
15. Preprocess candidate files only after approval.
16. Update page `3386e29b-97d5-814b-96f0-e5e4515e58c7` when `system_architecture.md` is a candidate.
17. For each weekly candidate, preprocess the file and build the page title as `N주차 — ...`.
18. Update the existing weekly page when `notion_page_id` exists.
19. Create a new weekly child page under `3386e29b-97d5-80dd-8546-f9877e9f82f3` when `notion_page_id` does not exist.
20. Save `mtime`, `content_hash`, and `notion_page_id` after each successful create or update.
21. For approved state-only updates, save `content_hash` without calling Notion.
22. Write the state file after all approved work finishes.

## Rules

- Do not expand the sync scope unless the user asks.
- Do not call Notion or write the state file before the user approves the printed candidate list.
- Do not create a new page when `notion_page_id` already exists.
- Do not change page title or parent location unless required for sync.
- Continue with other files even if one file fails.

## Output

- Print `동기화 완료`.
- Print `동기화 후보: ...`.
- Print `사용자 승인 대기: ...`.
- Print `업데이트: ...`.
- Print `생성: ...`.
- Print `건너뜀: ...`.
- Print `상태만 갱신: ...`.
- Print `실패: ...`.
