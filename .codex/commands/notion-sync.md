plant-monitor 프로젝트의 마크다운 파일을 Notion과 동기화한다.

## 목적

- 프로젝트 문서 중 Notion에 반영해야 하는 파일만 골라 동기화한다.
- 마지막 수정 시각(mtime)을 기준으로 변경된 파일만 처리한다.
- 주차별 문서는 완료 마커가 있는 파일만 업로드한다.

## 동기화 대상

1. `system_architecture.md`
   - Notion page ID: `3386e29b-97d5-814b-96f0-e5e4515e58c7`

2. `주차별_맥락/{N}주차_맥락.md`
   - Parent page ID: `3386e29b-97d5-80dd-8546-f9877e9f82f3`
   - `**{N}주차 완료**` 마커가 있는 파일만 동기화
   - 동기화 전 `## 현재 진행 상황` 섹션 완전 제거

## 상태 파일

- 상태 파일 경로: `{project_root}/.agent/notion_sync_state.json`
- 각 파일별 마지막 업로드 기준 `mtime`와 Notion page ID를 저장한다.
- 현재 파일의 `mtime`와 상태 파일 값을 비교해서 변경된 파일만 Notion API를 호출한다.

## 변환 규칙

1. ASCII art 플로우차트(`──`, `▶`, `│`, `▼` 등 포함)는 가능하면 Mermaid 플로우차트로 변환한다.
2. Mermaid 변환이 애매하거나 원형 보존이 더 안전하면 원문을 유지한다.
3. h1 제목(`# ...`)은 본문에서 제거한다. 페이지 제목은 Notion 페이지 제목으로 별도 사용한다.
4. `## 현재 진행 상황` 섹션은 하위 내용까지 포함해 완전히 제거한다.
5. 업로드 전 마크다운이 Notion Markdown으로 크게 깨지지 않는지 확인한다.

## 사용할 Codex 도구

- 페이지 조회: `mcp__codex_apps__notion_fetch`
- 페이지 생성: `mcp__codex_apps__notion_notion_create_pages`
- 페이지 수정: `mcp__codex_apps__notion_notion_update_page`

## 절차

1. `{project_root}/.agent/notion_sync_state.json`을 읽는다.
   - 파일이 없으면 빈 상태로 시작한다.

2. `system_architecture.md`를 확인한다.
   - 파일의 현재 `mtime`를 읽는다.
   - 상태 파일의 `mtime`와 다르면 본문을 전처리한 뒤 지정된 Notion page ID를 업데이트한다.
   - 업데이트 성공 시 상태 파일의 `architecture` 항목을 갱신한다.

3. `{project_root}/주차별_맥락/` 폴더의 모든 `*주차_맥락.md` 파일을 순회한다.
   - `**{N}주차 완료**` 마커가 없는 파일은 건너뛴다.
   - 파일의 현재 `mtime`를 읽는다.
   - 상태 파일과 비교해 변경된 파일만 처리한다.
   - 제목은 파일명과 첫 h1을 참고해 `N주차 — ...` 형식으로 맞춘다.
   - 기존 Notion page ID가 있으면 업데이트한다.
   - 기존 page ID가 없으면 Parent page 아래 새 페이지를 생성한다.
   - 생성 또는 업데이트 성공 시 해당 파일 항목의 `mtime`과 `notion_page_id`를 상태 파일에 기록한다.

4. 모든 동기화가 끝나면 상태 파일을 저장한다.

5. 마지막에 결과를 요약한다.
   - 업데이트한 파일
   - 새로 생성한 페이지
   - 건너뛴 파일과 이유
   - 실패한 항목

## 작업 세부 규칙

- 이미 Notion page ID가 있는 항목은 새 페이지를 만들지 말고 기존 페이지를 갱신한다.
- 새 페이지 생성이 필요한 경우, 생성 결과에서 page ID를 추출해 상태 파일에 저장한다.
- 본문 교체 전에는 기존 페이지 제목과 부모 위치를 불필요하게 바꾸지 않는다.
- 사용자가 별도로 요청하지 않으면 동기화 대상 범위를 늘리지 않는다.
- 실패한 항목이 있어도 가능한 파일은 계속 처리하고, 마지막에 실패 목록을 보고한다.

## 출력 형식

- `동기화 완료`
- `업데이트: ...`
- `생성: ...`
- `건너뜀: ...`
- `실패: ...`

## 참고

- 기존 Claude용 커맨드: `{project_root}/.claude/commands/notion-sync.md`
- 상태 파일: `{project_root}/.agent/notion_sync_state.json`
- 상태 파일은 Claude와 Codex가 공통으로 `{project_root}/.agent/notion_sync_state.json`을 사용한다.
