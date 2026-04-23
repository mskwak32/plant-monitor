Update the weekly context file at `주차별_맥락/{N}주차_맥락.md`.

## Week Selection

- Use `$ARGUMENTS` as the week number when provided.
- Example: `/update-week 5` means `5주차`.
- If `$ARGUMENTS` is missing, read `개발_일정_체크리스트.md` and determine the current in-progress week automatically.

## Run

1. Determine the target week number.
2. Check `주차별_맥락/{N}주차_맥락.md`.
3. Read the file if it already exists.
4. If the file does not exist, read the previous week's file for structure reference and create a new file.
5. Collect git history with `git log --oneline -20`.
6. Collect recent file changes with `git diff HEAD~3..HEAD --stat`.
7. Filter the collected git information to the commits relevant to the target week.
8. Check the current source structure, including `BSP`, `App`, and `Core`.
9. Identify newly added files.
10. Update the weekly context file.

## Update Content

- Update the current progress section with completed and incomplete items.
- Add CubeMX configuration changes made during the current session.
- Reflect what was covered in today's conversation.
- Include newly learned concepts and explanations.
- Include implemented features.
- Include discovered problems and how they were solved.
- Include physical wiring and test results.
- Update the file structure section.
- Expand the `이번 주 배운 것들` section.

## Writing Rules

- Follow the format of the previous weekly context file.
- Write the weekly context file in Korean.
- Leave incomplete items blank or mark them as `예정`.
- Include code blocks, diagrams, and other learning-helpful material when useful. Prefer Mermaid diagrams over ASCII art whenever the content can be expressed as a flowchart, sequence diagram, or graph.
- The `이번 주 배운 것들` section must be written in full detail. Do NOT summarize or compress concepts — write them out completely as if explaining to someone reading for the first time.
- Remove duplicates and trivial content, but never cut depth. Each concept should include: what it is, why it works that way, what breaks if you omit it, and analogies to Kotlin/Android where applicable.
- Keep enough detail so the document can be used later for study or as source material for a book-length writeup.
- Prefer to err on the side of too much detail rather than too little in the `이번 주 배운 것들` section.
- Do not include a "next week plan" section.
- In Mermaid diagrams, `\n` is not rendered. Use `<br>` for line breaks inside node labels.
