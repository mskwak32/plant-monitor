Sync user-specified markdown files from the plant-monitor project to Notion.

## Targets

- `system_architecture.md` -> page `3386e29b-97d5-814b-96f0-e5e4515e58c7`.
- `주차별_맥락/{N}주차_맥락.md`:
  - Weeks 1-7 -> child page under STM32 parent `3386e29b-97d5-80dd-8546-f9877e9f82f3`.
  - Weeks 8+ -> child page under RPi parent `3386e29b-97d5-8085-96c7-df544d93f3cf`.

## State

- Read `{project_root}/.agent/notion_sync_state.json`.
- Use an empty state if the file does not exist.
- Store `notion_page_id` for weekly files after create/update.
- `system_architecture.md` uses the fixed page ID above.

## Preprocess

- Remove the body h1 and use it only as the Notion title.
- Remove the entire `## 현재 진행 상황` section.
- Convert ASCII flowcharts to Mermaid when the structure is clear and reliable.
- Keep the original ASCII art when Mermaid conversion is unclear.
- When Mermaid conversion is unclear, also generate an SVG version in the same context folder as the source markdown.
- Check that the markdown will not break badly in Notion.

## Tools

- Use `mcp__codex_apps__notion_fetch` for page lookup.
- Use `mcp__codex_apps__notion_notion_create_pages` for new pages.
- Use `mcp__codex_apps__notion_notion_update_page` for existing pages.

## Run

1. Parse only the sync targets explicitly requested by the user.
2. Resolve `system_architecture.md` or "architecture" to `system_architecture.md`.
3. Resolve a weekly file path or week number such as `7주차` to `주차별_맥락/{N}주차_맥락.md`.
4. Stop and ask which file to sync if the user did not specify a supported target.
5. Skip a specified target if the source file does not exist.
6. Print the candidate list before any Notion create/update call.
7. Print the skip list for visibility.
8. Ask the user whether to proceed with the candidate sync.
9. Stop without calling Notion or writing the state file unless the user explicitly approves.
10. Preprocess candidate files only after approval.
11. Update page `3386e29b-97d5-814b-96f0-e5e4515e58c7` when `system_architecture.md` is a candidate.
12. For each weekly candidate, preprocess the file and build the page title as `N주차 — ...`.
13. Update the existing weekly page when `notion_page_id` exists.
14. Create a new weekly child page under the correct weekly parent when `notion_page_id` does not exist: weeks 1-7 use STM32 parent, weeks 8+ use RPi parent.
15. Save `notion_page_id` for each successfully synced weekly file.
16. Write the state file after all approved work finishes.

## Rules

- Sync only files explicitly specified by the user.
- Do not expand the sync scope unless the user asks.
- Do not create a new page when `notion_page_id` already exists.
- Do not change page title or parent location unless required for sync.
- Do not call Notion or write the state file before the user approves the printed candidate list.
- Continue with other files even if one file fails.
