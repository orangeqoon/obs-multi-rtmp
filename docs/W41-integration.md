# W41 統合メモ（obs-multi-rtmp）

## 版番号
- `list_capabilities` に `apiVersion`（整数、今回 `2`）と `pluginVersion`（buildspec の version）を追加。
- 新規リクエスト `get_api_version` は同じ 2 フィールドだけ返す。
- marust は起動時にどちらかで版を確認し、未対応ならエンジン操作を無効にすること。

## ヘッドレス
- 設定キー（プロファイルの `obs-multi-rtmp.json`）: `"hide_dock": true|false`（既定 false）。
- UI 文言: 「ドックを出さない（marust から操作）」／ `Setting.HideDock`。
- ON のとき配信先一覧ドックを隠し、非常停止だけのドックを出す。OFF のときは一覧ドック（中に非常停止ボタン付き）だけを出し、単独の非常停止ドックは隠す。

## 非常停止（D7）
- 青（配信中 N 件）: 1 秒長押し → 全配信先を `ForceStopStreaming`（`stop_all_targets` と同じ経路）。
- 赤（全停止）: 押しても何もしない。
- イベント: 各配信先の `target_state_changed` に加え、`emergency_stop`（`time` / `stopped_ids` / `count`）。
- marust 無しでもエンジン内だけで動作。

## target_state_changed 追加フィールド
- `reconnect_count`（開始ごとに 0、再接続のたびに +1）
- 既存の `last_error_code` を継続利用（§6.3 の「最後のエラー」）

## テスト
- 既存の C++ 単体テスト枠は無かったため、`tests/json-api-check.cpp` を追加。
- `WEBSOCKET_API.md` の JSON を組み立てる純関数（`src/websocket-api-json.hpp`）をビルド時に実行確認する。

## アーキテクチャとの差分
- §6.3 項 4（配信先ごとの自動再接続方針を API で設定）は W41 タスク範囲外のため未実装。後続波に回す。