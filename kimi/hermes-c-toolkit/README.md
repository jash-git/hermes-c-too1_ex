# hermes-c-toolkit

讓 Hermes 呼叫使用的純 C 簡易工具 + SKILL 專案。

## 結構

```
hermes-c-toolkit/
├── SKILL.md          # Hermes 讀取的技能定義（frontmatter + 指令說明）
├── README.md         # 本檔案
├── src/
│   └── ctool.c       # 純 C99 單檔工具（無外部依賴）
├── scripts/
│   └── build.sh      # 編譯腳本（cc/gcc/clang）
├── bin/
│   └── ctool         # 預編譯 Linux x86_64 執行檔
└── examples/
    └── sample.txt    # 測試用範例檔
```

## 快速開始

```bash
sh scripts/build.sh                      # 重新編譯（可選）
bin/ctool calc "(3+4)*5/2 - 10%3"        # → 16.5
bin/ctool stats examples/sample.txt      # 行數/字數/位元組統計
bin/ctool b64e "Hello, Hermes!"          # → SGVsbG8sIEhlcm1lcyE=
bin/ctool hash examples/sample.txt       # FNV-1a 64 雜湊
```

## 安裝到 Hermes

把整個 `hermes-c-toolkit/` 目錄複製到 Hermes 的技能目錄（例如 `~/.hermes/skills/`），
Hermes 掃描到 `SKILL.md` 後即可依 description 觸發並呼叫 `bin/ctool`。

## 測試結果

全部 8 個子命令通過，輸出與 Python 參照實作一致（base64、算式結果、FNV-1a）；
錯誤案例（除以零、非法 base64、語法錯誤）正確回傳 exit 1 並輸出 stderr 訊息。
