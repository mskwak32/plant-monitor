Sync selected markdown files from the plant-monitor project to Notion.

## Targets

- Sync `system_architecture.md` to page `3386e29b-97d5-814b-96f0-e5e4515e58c7`.
- Sync `주차별_맥락/{N}주차_맥락.md` under parent `3386e29b-97d5-80dd-8546-f9877e9f82f3`.
- Sync weekly files only when they contain `**{N}주차 완료**`.

## State

- Read `{project_root}/.agent/notion_sync_state.json`.
- Use an empty state if the file does not exist.
- Store `mtime` and `notion_page_id` for each synced file.
- Call Notion only when the current `mtime` differs from the saved `mtime`.

## Preprocess

- Remove the body h1 and use it only as the Notion title.
- Remove the entire `## 현재 진행 상황` section.
- Convert ASCII flowcharts to Mermaid when the structure is clear and the result is reliable.
- Keep the original ASCII art in the Notion-synced markdown when Mermaid conversion is unclear.
- When Mermaid conversion is unclear, also generate an SVG version and save it in the same context folder as the source markdown.
- Check that the markdown will not break badly in Notion.

## Tools

- Use `mcp__claude_ai_Notion__notion-fetch` for page lookup.
- Use `mcp__claude_ai_Notion__notion-create-pages` for new pages.
- Use `mcp__claude_ai_Notion__notion-update-page` for existing pages.

## Run

1. Read the state file.
2. Check `system_architecture.md`.
3. Skip it if `mtime` is unchanged.
4. Preprocess it if `mtime` changed.
5. Update page `3386e29b-97d5-814b-96f0-e5e4515e58c7`.
6. Save the new `mtime` after a successful update.
7. Iterate all `주차별_맥락/*주차_맥락.md` files.
8. Skip files without `**{N}주차 완료**`.
9. Skip files whose `mtime` is unchanged.
10. Build the page title as `N주차 — ...`.
11. Update the existing page when `notion_page_id` exists.
12. Create a new child page under `3386e29b-97d5-80dd-8546-f9877e9f82f3` when `notion_page_id` does not exist.
13. Save `mtime` and `notion_page_id` after each successful create or update.
14. Write the state file after all work finishes.

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
