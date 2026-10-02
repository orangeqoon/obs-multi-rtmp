# obs-multi-rtmp (Automated & Scriptable Edition)

[![Release](https://img.shields.io/github/v/release/orangeqoon/obs-multi-rtmp?color=blue&label=release)](https://github.com/orangeqoon/obs-multi-rtmp/releases/tag/0.7.5.1)
[![Build Status](https://img.shields.io/github/actions/workflow/status/orangeqoon/obs-multi-rtmp/build.yml?branch=master)](https://github.com/orangeqoon/obs-multi-rtmp/actions)
[![License: GPL-2.0](https://img.shields.io/badge/License-GPL%202.0-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Ubuntu%20%7C%20Flatpak-lightgrey)](https://github.com/orangeqoon/obs-multi-rtmp/releases/tag/0.7.5.1)

> **OBSによる複数・同時配信を、外部スクリプトやAIから完全自動化。**  
> 定番プラグイン [sorayuki/obs-multi-rtmp](https://github.com/sorayuki/obs-multi-rtmp) の複数配信・エンコーダー共有機能はそのままに、**obs-websocket 外部制御 API** や **自動化向け安全弁** を追加した実戦向けフォークです。

---

## 🚀 本家との違い・機能強化

本家の基本機能（RTMP/SRT/WHIP同時配信、同期start/stop、エンコーダー共有）を完全維持した上で、以下の機能を追加・強化しています。

- **obs-websocket 外部制御 API**:
  ドックを開かずに外部から完全制御。計11種のリクエスト（`list_targets`, `start_target`, `stop_target`, `set_target_service_settings` など）と `target_state_changed` リアルタイムイベントに対応。（全仕様: [WEBSOCKET_API.md](./WEBSOCKET_API.md)）
- **自動化向けの安全弁**:
  キー等の機密情報はデフォルトでマスク（`***redacted***`）、誤爆を防ぐ事前検証（`dry_run`）、新規作成ターゲットの初期無効化、配信中削除の拒否（要 `confirm`）。
- **ターゲットの一時無効化 ＆ 状態色分け**:
  チェックボックスで配信先を個別にオン/オフ。●の色（灰:停止 / 黄:接続中 / 緑:配信中 / 赤:エラー）で状態を一目で把握。
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

`obsws-python` 等を使い、数行で配信先の監視・キー更新・制御が可能です。

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

A scriptable and automated fork of [sorayuki/obs-multi-rtmp](https://github.com/sorayuki/obs-multi-rtmp).
- **obs-websocket Vendor API**: 11 requests (`list_targets`, `start_target`, `set_target_service_settings`, etc.) and `target_state_changed` event. (Specs: [WEBSOCKET_API.md](./WEBSOCKET_API.md))
- **Safety Valves**: Secret redaction, `dry_run` validation, disabled-by-default for new targets, safe deletion.
- **UI Enhancements**: Enable/disable checkboxes per target, color-coded indicators, JSON config export/import.
- **OBS Hotkeys**: Bind Start/Stop actions per target or globally.
- **Crash Fix**: Fixed race condition during connection re-entry (v0.7.5.1).

</details>

---

## 📜 ライセンス & クレジット

- ライセンス: [GNU General Public License v2.0](LICENSE)
- オリジナル作者: [sorayuki](https://github.com/sorayuki) ([sorayuki/obs-multi-rtmp](https://github.com/sorayuki/obs-multi-rtmp))
- 外部制御API・拡張: [orangeqoon](https://github.com/orangeqoon)
