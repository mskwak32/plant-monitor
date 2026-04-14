---
name: plant-monitor-week-planner
description: Plan the next step for the Plant Monitor portfolio project from weekly context. Use when the user says phrases such as "{N}주차 시작", "{N} 주차 시작", "{N}주차 이어서 가보자", or asks to continue a specific project week; read AGENTS.md and project planning/context files, summarize current status and next work, and wait for user confirmation before coding.
---

# Plant Monitor Week Planner

## Workflow

1. Identify the target week number from the user's latest message.
2. Read the applicable `AGENTS.md` instructions before interpreting or changing project files.
4. Read `개발_일정_체크리스트.md` to find planned tasks and the current week-level progress.
5. Check `주차별_맥락/{N}주차_맥락.md`.
6. If `주차별_맥락/{N}주차_맥락.md` exists, read it and treat the request as continuing that week.
7. If `주차별_맥락/{N}주차_맥락.md` does not exist, treat the request as starting that week.
8. Summarize the current status, relevant context, likely blockers, and the next recommended steps.
9. Stop before editing code or project files, and wait for the user's response.

## Response Rules

- Respond in Korean using formal polite speech.
- Do not begin implementation immediately, even if the next task looks obvious.
- Mention whether the week context file exists or is being treated as a new week.
- Keep the first response focused on orientation: what is done, what is next, and what decision or confirmation is needed from the user.
- If the next work may affect STM32 and Raspberry Pi boundaries, call that out explicitly and keep code in the correct project folder after confirmation.
