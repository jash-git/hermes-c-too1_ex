---
name: hermes-c-tool
description: 使用可由 Hermes 呼叫的純 C 數值計算 CLI。當需要在 Linux sandbox 以低依賴、可重現且 JSON 輸出的方式計算一組數字的總和、平均值、最小值或最大值時使用。
---

# Hermes C Tool

使用內附的 `scripts/run.sh` 呼叫純 C 工具；首次呼叫會自動以本機 C 編譯器建置，後續直接重用 `build/hermes-calc`。不要直接依賴目前工作目錄，請使用本 `SKILL.md` 所在 Skill 目錄的絕對路徑。

## 呼叫協定

```sh
<skill-dir>/scripts/run.sh <operation> <number> [<number> ...]
```

支援的 operation：

- `sum`：計算總和
- `avg` 或 `average`：計算算術平均
- `min`：取最小值
- `max`：取最大值

也可以省略命令列數字，改由 stdin 傳入空白分隔的數字：

```sh
printf '10 20 30\n' | <skill-dir>/scripts/run.sh sum
```

## 輸出與錯誤處理

工具永遠輸出單行 JSON。成功格式為：

```json
{"ok":true,"operation":"sum","count":3,"result":60}
```

失敗格式為：

```json
{"ok":false,"error":"no numbers supplied"}
```

以程序退出碼判斷結果：`0` 代表成功；`2` 代表參數、數字或 operation 錯誤；`3` 代表記憶體錯誤。解析 JSON 後再向使用者回覆，不要把編譯器輸出當成計算結果。

## 典型流程

1. 將使用者要求轉成一個 operation 與數字陣列。
2. 使用 `scripts/run.sh`，優先以命令列參數傳入少量數字；大量數字使用 stdin。
3. 讀取唯一一行 JSON，確認 `ok` 為 `true`。
4. 若 `ok` 為 `false`，根據 `error` 修正輸入，不要猜測結果。

## 限制

此工具只接受有限的十進位浮點數，不接受 `NaN`、無限大、數學運算式或逗號分隔格式。計算使用 `double`；需要任意精度、貨幣精度或符號運算時，不要使用此工具。
