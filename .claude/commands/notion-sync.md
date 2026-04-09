plant-monitor 프로젝트의 마크다운 파일을 Notion과 동기화합니다.

## 동기화 대상

1. **system_architecture.md** (프로젝트 루트)
   - Notion page ID: 3386e29b-97d5-814b-96f0-e5e4515e58c7

2. **주차별_맥락/{N}주차_맥락.md** (주차별_맥락/ 폴더)
   - Parent page ID: 3386e29b-97d5-80dd-8546-f9877e9f82f3 (plant-monitor-stm32)
   - `**{N}주차 완료**` 마커가 있는 파일만 동기화
   - 동기화 전 `## 현재 진행 상황` 섹션 제거

## 상태 파일

프로젝트 루트 `{project_root}/.agent/notion_sync_state.json`에 각 파일의 마지막 mtime을 기록.
현재 mtime과 비교해서 변경된 파일만 Notion API 호출.

## 변환 규칙

1. ASCII art 플로우차트(──, ▶, │, ▼ 포함) → 가능하면 Mermaid 플로우차트로 변환.
2. Mermaid 변환이 애매하거나 원형 보존이 더 안전하면 원문을 유지한다.
3. h1 제목(`# ...`) 제외 (Notion 페이지 타이틀로 별도 설정됨)
4. `## 현재 진행 상황` 섹션 완전 제거 후 업로드

## 절차

1. `{project_root}/.agent/notion_sync_state.json` 읽기
2. `{project_root}/system_architecture.md` mtime 확인 → 변경 시 Notion 업데이트
3. `{project_root}/주차별_맥락/` 폴더의 모든 `*주차_맥락.md` 파일 순회
   - `**{N}주차 완료**` 마커 있는 파일만 처리
   - mtime 변경 시 Notion 업데이트 (신규면 페이지 생성, 기존이면 업데이트)
4. 상태 파일 갱신
5. 동기화 결과 요약 출력
