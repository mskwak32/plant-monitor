Sync selected markdown files from the plant-monitor project to Notion.

## Targets

- Sync `system_architecture.md` to page `3386e29b-97d5-814b-96f0-e5e4515e58c7`.
- Sync `주차별_맥락/{N}주차_맥락.md` under parent `3386e29b-97d5-80dd-8546-f9877e9f82f3`.
- Sync weekly files only when they contain `**{N}주차 완료**`.

## State

- Read `{project_root}/.agent/notion_sync_state.json`.
- Use an empty state if the file does not exist.
- Store `mtime`, `content_hash`, and `notion_page_id` for each synced file.
- Compute `content_hash` as SHA-256 of the preprocessed markdown that will be sent to Notion, not the raw source file.
- Use `mtime` only as a fast change check.
- Call Notion only when the current preprocessed `content_hash` differs from the saved `content_hash`.
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
4. Preprocess it if `mtime` changed or `content_hash` is missing.
5. Compute SHA-256 `content_hash` from the preprocessed markdown.
6. Skip Notion and save only the new `mtime` when `content_hash` is unchanged.
7. Update page `3386e29b-97d5-814b-96f0-e5e4515e58c7` when `content_hash` changed.
8. Save the new `mtime` and `content_hash` after a successful update.
9. Iterate all `주차별_맥락/*주차_맥락.md` files.
10. Skip files without `**{N}주차 완료**`.
11. Skip files whose `mtime` is unchanged and `content_hash` exists.
12. Preprocess changed weekly files, or legacy weekly files missing `content_hash`, and compute SHA-256 `content_hash`.
13. Skip Notion and save only the new `mtime` when `content_hash` is unchanged.
14. For legacy entries with unchanged `mtime` and missing `content_hash`, save `content_hash` without calling Notion.
15. Build the page title as `N주차 — ...` when `content_hash` changed.
16. Update the existing page when `notion_page_id` exists.
17. Create a new child page under `3386e29b-97d5-80dd-8546-f9877e9f82f3` when `notion_page_id` does not exist.
18. Save `mtime`, `content_hash`, and `notion_page_id` after each successful create or update.
19. Write the state file after all work finishes.

## Rules

- Do not expand the sync scope unless the user asks.
- Do not create a new page when `notion_page_id` already exists.
- Do not change page title or parent location unless required for sync.
- Continue with other files even if one file fails.

## Output

- Print `동기화 완료`.
- Print `업데이트: ...`.
- Print `생성: ...`.
- Print `건너뜀: ...`.
- Print `실패: ...`.
