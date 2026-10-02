# obs-multi-rtmp (AI & Script Automation Edition)

[![Release](https://img.shields.io/github/v/release/orangeqoon/obs-multi-rtmp?color=blue&label=release)](https://github.com/orangeqoon/obs-multi-rtmp/releases/tag/0.7.5.1)
[![Build Status](https://img.shields.io/github/actions/workflow/status/orangeqoon/obs-multi-rtmp/build.yml?branch=master)](https://github.com/orangeqoon/obs-multi-rtmp/actions)
[![License: GPL-2.0](https://img.shields.io/badge/License-GPL%202.0-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Ubuntu%20%7C%20Flatpak-lightgrey)](https://github.com/orangeqoon/obs-multi-rtmp/releases/tag/0.7.5.1)

> **OBSの複数同時配信を、AIエージェントや外部スクリプトから安全に完全自動化。**  
> 定番プラグイン [sorayuki/obs-multi-rtmp](https://github.com/sorayuki/obs-multi-rtmp) の複数配信・エンコーダー共有機能はそのままに、**AIコーディング・自律エージェント運用に最適化された外部制御 API と事故防止ガードレール**を組み込んだ実戦向けフォークです。

---

## 🤖 AI・スクリプト連携に特化した機能強化（本家との違い）

本家の基本機能（RTMP/SRT/WHIP同時配信、同期start/stop、エンコーダー共有）を完全維持した上で、**AIが迷わず・事故を起こさず自律制御できる仕組み**を追加しています。

- **AIが自己探索できる obs-websocket API (`list_capabilities`)**:
  AIが最初に `list_capabilities` を呼ぶだけで全コマンド仕様と引数を自己把握可能。長いドキュメントをプロンプトに詰め込む必要がありません。ドック操作なしで計11種のリクエスト制御と `target_state_changed` リアルタイムイベントに対応。（全仕様: [WEBSOCKET_API.md](./WEBSOCKET_API.md)）
- **AI運用を前提とした安全弁（ガードレール）**:
  - **配信キーの漏洩防止**: `get_target_config` 取得時、キーやトークンは自動で `***redacted***` にマスクされ、プロンプトやログへの流出を防止。
  - **事前シミュレーション (`dry_run`)**: 変更や作成を実際に反映せずパラメータ妥当性だけを検証可能。
  - **誤配信防止のデフォルト無効化**: API経由の新規作成ターゲットは `enabled: false`（無効）で作成。
  - **配信中削除の拒否**: 稼働中の配信先削除は拒否（`target_is_live`、要 `confirm: true`）。
- **ターゲットの一時無効化 ＆ 状態色分け**:
  チェックボックスで配信先を個別に除外可能。●の色（灰:停止 / 黄:接続中 / 緑:配信中 / 赤:エラー）で状態を一目で把握。
- **設定のエクスポート / インポート**:
  配信キーを含む全設定をJSONでバックアップ・別環境へ一括移行可能。
- **OBS ホットキー対応**:
  各配信先および全体のStart/StopをOBSの「設定 → ホットキー」からキーバインド可能。
- **レースコンディション・クラッシュ修正**:
  接続処理中の再入によるクラッシュ不具合を修正し、長時間運用の安定性を向上（v0.7.5.1）。

---

## 📦 インストールと動作環境

全プラットフォーム（Windows, macOS, Ubuntu, Flatpak）向けビルドを GitHub Actions CI で検証済みです。

### 推奨リリース
- **[v0.7.5.1 Release](https://github.com/orangeqoon/obs-multi-rtmp/releases/tag/0.7.5.1)**  
  *(※ 0.7.5.0 は潜在的なクラッシュがあるため非推奨です。必ず v0.7.5.1 をご利用ください)*

### Windows での導入手順
1. [Releases](https://github.com/orangeqoon/obs-multi-rtmp/releases/tag/0.7.5.1) から `obs-multi-rtmp-0.7.5.1-windows-x64.zip` またはインストーラーを入手。
2. OBS Studio を終了し、ZIPの内容をプラグインフォルダ（通常は `C:\ProgramData\obs-studio\plugins\` または OBS インストール先）に展開。
3. OBS Studio を起動すると、ドックに「複数配信」が追加されます。

---

## 🛠️ スクリプト・外部からの利用例 (Python)

`obsws-python` 等を使い、AIやスクリプトから数行で配信先の監視・キー更新・制御が可能です。

```python
import obsws_python as obs

cl = obs.ReqClient(host='localhost', port=4455, password='your_obs_password')

# 1. 配信先一覧を取得
targets = cl.call_vendor_request(vendor_name="obs-multi-rtmp", request_type="list_targets")
print(targets.responseData["targets"])

# 2. 配信キーを更新して即座に再接続 (YouTube 12時間ロールオーバー等)
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
<summary><strong>English (Summary)</strong></summary>

A scriptable and AI-agent friendly fork of [sorayuki/obs-multi-rtmp](https://github.com/sorayuki/obs-multi-rtmp).
- **AI-Ready obs-websocket API**: Self-describing `list_capabilities` API, 11 vendor requests (`list_targets`, `start_target`, `set_target_service_settings`, etc.), and `target_state_changed` event. (Specs: [WEBSOCKET_API.md](./WEBSOCKET_API.md))
- **Safety Valves / Guardrails**: Stream key redaction (`***redacted***`) by default, `dry_run` simulation, disabled-by-default for new targets, safe live-deletion protection.
- **UI Enhancements**: Enable/disable checkboxes per target, color-coded indicators, JSON config export/import.
- **OBS Hotkeys**: Bind Start/Stop actions per target or globally.
- **Crash Fix**: Fixed race condition during connection re-entry (v0.7.5.1).

</details>

---

## 📜 ライセンス & クレジット

- ライセンス: [GNU General Public License v2.0](LICENSE)
- オリジナル作者: [sorayuki](https://github.com/sorayuki) ([sorayuki/obs-multi-rtmp](https://github.com/sorayuki/obs-multi-rtmp))
- 外部制御API・拡張: [orangeqoon](https://github.com/orangeqoon)
