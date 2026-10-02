# obs-multi-rtmp (Automated & Scriptable Edition)

[![Release](https://img.shields.io/github/v/release/orangeqoon/obs-multi-rtmp?color=blue&label=release)](https://github.com/orangeqoon/obs-multi-rtmp/releases/tag/0.7.5.1)
[![Build Status](https://img.shields.io/github/actions/workflow/status/orangeqoon/obs-multi-rtmp/build.yml?branch=master)](https://github.com/orangeqoon/obs-multi-rtmp/actions)
[![License: GPL-2.0](https://img.shields.io/badge/License-GPL%202.0-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Ubuntu%20%7C%20Flatpak-lightgrey)](https://github.com/orangeqoon/obs-multi-rtmp/releases/tag/0.7.5.1)

> **OBSによる超多重・同時配信を、完全自動化へ。**  
> 定番プラグイン「[sorayuki/obs-multi-rtmp](https://github.com/sorayuki/obs-multi-rtmp)」の複数配信機能・エンコーダー共有性能はそのままに、**obs-websocket 経由のフル外部制御 API**、**AI・スクリプト連携用の安全弁機構**、**ホットキー**、**設定バックアップ**、**潜在クラッシュの修正**を施した実戦投入版フォークです。

---

## 💡 なぜこのフォークが必要だったのか？ (開発背景)

YouTube, Twitch, ニコニコ生放送, ツイキャス, Kick, FC2, SOOP, Picarto, Rumble など、**10以上のプラットフォームへ日常的に同時配信を行うヘビーユース環境**において、従来の `obs-multi-rtmp` には決定的なボトルネックがありました。

1. **GUI（ドック操作）への依存**: 人間がマウスでドックの各ボタンをクリックしなければ開始・停止・設定変更ができず、外部スクリプトやタイマー（NeonTimerApp等）から遠隔操作できなかった。
2. **自動ロールオーバー・再接続の困難**: YouTubeの長時間配信（12時間制限）に伴う配信キーのローテーションや、特定プラットフォーム切断時の自動復旧を外部プログラムから制御する口が存在しなかった。
3. **誤操作のリスク**: 一括配信時に特定の配信先だけをサッと除外する手段や、誤って削除・操作してしまうリスクへの安全設計が不足していた。
4. **潜在的なレースコンディション**: 接続処理中の再入によるクラッシュ不具合が存在した。

本プロジェクトは、これらの課題を根本から解決し、**「外部スクリプトやAIエージェントから安全・確実に完全自動制御できる obs-multi-rtmp」** として再設計・強化したものです。

---

## 🚀 主な強化機能・本家との違い

### 1. obs-websocket 外部制御 API (Vendor Requests & Events)
OBSドックを開くことなく、Python、Node.js、AIエージェント、外部自動化スクリプト等から配信ターゲットを完全にコントロール可能です。
- **全11種のリクエストAPI**:
  - `list_capabilities` / `list_targets` / `get_target_status` / `get_target_config`
  - `start_target` / `stop_target` / `start_all_targets` / `stop_all_targets`
  - `create_target` / `delete_target` / `set_target_service_settings` / `set_target_enabled`
- **リアルタイムイベント通知**:
  - `target_state_changed`: 各配信先が `connecting`, `live`, `reconnecting`, `stopped` に遷移した瞬間、即座にイベントを受信可能。ログファイルの泥臭いパースは不要です。
- 📖 完全なリクエスト・レスポンス仕様は [WEBSOCKET_API.md](./WEBSOCKET_API.md) をご覧ください。

### 2. 徹底した安全弁 (Safety Valves for Automation)
AIやスクリプトによる自動実行を前提とし、事故を未然に防ぐ防御層を組み込みました。
- **認証情報の自動秘匿（Redaction）**: `get_target_config` では、ストリームキーやトークン等の機密情報はデフォルトで `***redacted***` にマスク。ログやAIのプロンプト・チャットへの誤露出を防止（`reveal_secrets: true` の明示指定時のみ取得可能）。
- **安全な事前検証 (`dry_run`)**: `create_target` や `set_target_service_settings` では変更を実際に行わずにパラメータ妥当性を検証する `dry_run: true` をサポート。
- **誤爆防止のデフォルト無効化**: API経由で新規作成されたターゲットはデフォルトで `enabled: false`（無効状態）。設定を確認した後に明示的に有効化するフローを強制。
- **配信中削除の絶対拒否 & 確認フラグ**: 配信中のターゲットの `delete_target` は拒否 (`target_is_live`)。削除実行には `confirm: true` が必須。

### 3. 個別ターゲットの一時無効化 & ステータス色分け表示
- **チェックボックスで簡単除外**: 配信先リストの各ターゲットに有効/無効チェックボックスを追加。削除することなく「今日の配信はYouTubeとTwitchだけに絞る」といった切り替えがワンクリック、またはAPIから一瞬で行えます。
- **視認性の向上**: インジケータの丸印を状態別にカラーリング（灰: 停止中、黄: 接続中、緑: 配信中、赤: エラー/再接続中）。

### 4. 設定の JSON エクスポート / インポート
- ドック内のUIから、登録された全ターゲットの設定（配信URL、キー、エンコーダー割り当てなど）をJSONファイルとして一括バックアップ＆復元可能。
- 環境の引っ越しやPC更新、プロファイル別の切り替えが容易になりました。

### 5. ホットキー（OBS Hotkeys）完全対応
- OBSの「設定」→「ホットキー」に、各ターゲットごとの「配信開始/停止」、および全体の「全ターゲット配信開始/全ターゲット配信停止」を割り当て可能。
- Stream Deck やフットスイッチ、外部マクロパッドからの瞬時制御に対応します。

### 6. レースコンディション・クラッシュの修正
- 本家に存在した、接続試行中の `StartStreaming()` の再入によって発生する致命的なクラッシュ（レースコンディション）を修正し、長時間の連続運用にも耐える安定性を確保しました（v0.7.5.1）。

---

## 📦 インストールと動作環境

全プラットフォーム（Windows, macOS, Ubuntu, Flatpak）向けビルドが GitHub Actions CI により検証済みです。

### 推奨リリース
- **[v0.7.5.1 Release](https://github.com/orangeqoon/obs-multi-rtmp/releases/tag/0.7.5.1)**
  *(※ 0.7.5.0 は潜在的なクラッシュバグが存在するため非推奨です。必ず v0.7.5.1 以降をご利用ください)*

### Windows でのインストール手順
1. [Releases](https://github.com/orangeqoon/obs-multi-rtmp/releases/tag/0.7.5.1) から `obs-multi-rtmp-0.7.5.1-windows-x64.zip` またはインストーラーをダウンロードします。
2. OBS Studio を終了します。
3. 手動インストールの場合は、ZIPの中身を OBS のプラグインディレクトリ（通常は `C:\ProgramData\obs-studio\plugins\` または OBS インストールフォルダ）に展開します。
4. OBS Studio を起動すると、ドックメニューに「複数配信」が追加されます。

---

## 🛠️ スクリプト・外部からの利用例 (Python)

obs-websocket-py などを利用して、わずか数行でターゲットの制御が可能です。

```python
import obsws_python as obs

cl = obs.ReqClient(host='localhost', port=4455, password='your_obs_password')

# 1. 登録されている配信先の一覧を取得
targets_res = cl.call_vendor_request(vendor_name="obs-multi-rtmp", request_type="list_targets")
print(targets_res.responseData["targets"])

# 2. YouTubeの配信キーを自動更新して再接続 (12時間ロールオーバー等)
cl.call_vendor_request(
    vendor_name="obs-multi-rtmp",
    request_type="set_target_service_settings",
    request_data={
        "id": "target_id_here",
        "settings": {"key": "new_stream_key_xxxx"},
        "restart_if_active": True
    }
)

# 3. 全配信先の一括停止
cl.call_vendor_request(vendor_name="obs-multi-rtmp", request_type="stop_all_targets")
```

---

<details>
<summary><strong>English (Overview & Features)</strong></summary>

### Overview
A scriptable and automated fork of [sorayuki/obs-multi-rtmp](https://github.com/sorayuki/obs-multi-rtmp). It preserves all core features (multi RTMP/SRT/WHIP streaming, encoder sharing, sync start/stop) while adding **obs-websocket vendor API control**, **safety valves for automation**, **hotkeys**, **config export/import**, and **crash fixes**.

### Key Differences & Additions
- **obs-websocket Vendor API**: Control targets via 11 vendor requests (`list_targets`, `start_target`, `stop_target`, `create_target`, `delete_target`, `set_target_service_settings`, `set_target_enabled`, etc.) and subscribe to `target_state_changed` events without touching the Qt dock. Full specs in [WEBSOCKET_API.md](./WEBSOCKET_API.md).
- **Safety Valves**: Mask secrets (`***redacted***`) by default in config queries, `dry_run` support for validation, newly created targets start disabled, and live targets cannot be deleted without confirmation.
- **Enable/Disable Checkbox & Color Indicators**: Temporarily exclude targets without deleting them; color-coded indicators (gray/yellow/green/red) for immediate visual feedback.
- **Config Export / Import**: Backup and restore all target configurations via JSON from the dock UI.
- **Hotkeys**: Bind per-target and global Start/Stop actions in OBS's `Settings > Hotkeys`.
- **Race Condition & Crash Fix**: Fixed a critical crash caused by `StartStreaming()` re-entry during connection attempts (v0.7.5.1).

</details>

---

## 📜 ライセンス & クレジット

- 本プロジェクトは [GNU General Public License v2.0](LICENSE) の下で公開されています。
- オリジナル開発者: [sorayuki](https://github.com/sorayuki) ([sorayuki/obs-multi-rtmp](https://github.com/sorayuki/obs-multi-rtmp))
- 拡張・自動化API開発: [orangeqoon](https://github.com/orangeqoon)
