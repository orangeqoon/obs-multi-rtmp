# marust エンジン（この fork）について

このリポジトリは [sorayuki/obs-multi-rtmp](https://github.com/sorayuki/obs-multi-rtmp) の fork です。
**送出の中核**（複数配信先への出力・エンコーダー共有・再接続）はそのまま使い、
marust から操作するための外側（WebSocket の形・ドック・ヘッドレス）だけを足しています。

モジュール名・vendor 名（`obs-multi-rtmp`）・設定ファイルの場所（プロファイル内の
`obs-multi-rtmp.json`）は **upstream と同じ**です。変えると既存の配信先設定が使えなくなり、
upstream の取り込みも難しくなります。

## fork で触っている／足しているファイル

### 追加（marust 用・新規）
- `src/marust-snapshot.cpp` / `src/marust-snapshot.h` / `src/marust-snapshot-json.hpp` — `get_snapshot`
- `src/marust-shell.cpp` / `src/marust-shell.h` — Tools メニューとヘッドレス時のドック名
- `src/emergency-stop-widget.cpp` / `.h` — 非常停止ボタン（D7）
- `src/websocket-api-json.hpp` — vendor JSON の純関数
- `tests/json-api-check.cpp` — ビルド時の JSON 形チェック
- `docs/W41-integration.md` — W41 統合メモ
- `MARUST.md` — 本ファイル
- `WEBSOCKET_API.md` — vendor API ドキュメント（fork で追記）

### 既存ファイルへの小さなフック（呼び出し数行程度）
- `src/websocket-api.cpp` / `.h` — 版番号・イベント・`RegisterMarustVendorRequests` 呼び出し
- `src/obs-multi-rtmp.cpp` — 非常停止ドック／`RegisterMarustShell`／`SyncMarustShellUi`
- `src/push-widget.cpp` / `.h` — `reconnect_count` 通知
- `src/output-config.cpp` / `.h` — `hide_dock`
- `CMakeLists.txt` / `.gitignore` / `data/locale/en-US.ini` / `ja-JP.ini`

中核のままにしておきたいもの（upstream 追従時に衝突しやすい）:
`push-widget.cpp`・`output-config.cpp`・`protocols.cpp`・`edit-widget.cpp`・
エンコード／出力開始まわり。

## upstream の修正を取り込む手順

作業用ブランチ上で（例）:

```text
git fetch upstream
git merge upstream/master
```

衝突しやすい場所:
1. `src/websocket-api.cpp` — リクエスト表・`RegisterWebsocketVendor`
2. `src/obs-multi-rtmp.cpp` — `obs_module_load` のドック登録
3. `src/push-widget.cpp` — 状態通知・`OnReconnect` / `OnStopped`
4. `src/output-config.*` — JSON の save/load
5. `CMakeLists.txt` / `data/locale/*`

marust 側の新規ファイル（`src/marust-*`）は upstream に無いので、通常は衝突しません。
中核のロジック衝突は **upstream 側を優先し、こちらの「呼び出し 1〜2 行」だけ付け直す**のが安全です。

（このドキュメントに書いた git コマンドは手順の説明です。実行は人手で行ってください。）

## ビルド手順（Windows x64）

前提: Visual Studio 2022 Build Tools（C++）と、付属の CMake。

```text
cmake --preset windows-x64
cmake --build --preset windows-x64
```

- 設定・ビルド成果: `build_x64/`
- プラグイン DLL（RelWithDebInfo）:
  `build_x64\RelWithDebInfo\obs-multi-rtmp.dll`

初回 configure 時に OBS ソースと依存 zip を `.deps/` へ取得します。
**出来た dll を OBS のプラグインフォルダへ自動コピーしないでください**（ユーザーが配信していないときに入れ替え）。

## marust 向けの要点

- vendor 名: `obs-multi-rtmp`（変更しない）
- `apiVersion`: 3（`list_capabilities` / `get_api_version` / `get_snapshot`）
- 突き合わせ: `get_snapshot`（秘密は含まない）
- ヘッドレス: 設定 `hide_dock`。Tools メニュー
  「marust エンジン: 配信先の一覧ドックを表示」で一覧ドックに戻れる
- ヘッドレス時の非常停止ドック名: 「marust エンジン」