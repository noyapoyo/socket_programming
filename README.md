# Socket Programming Project

## 簡介
此專案是一個基於 socket 的簡單伺服器-客戶端應用程式，支援用戶註冊、登入等功能，並使用 C++ 實作。

---

## 環境設置

要執行 `socket_programming` 專案的伺服器和客戶端程式，您需要建立相容的環境、安裝必要的依賴項，並進行編譯。

### 需求

#### 必要軟體
1. **編譯器**：`g++`（用於 C++ 編譯）。
2. **作業系統**：Unix-based 系統（如 Linux）。
   - 如果使用 Windows，可以使用 WSL（Windows Subsystem for Linux）來模擬 Unix-based 環境。
3. **網路工具**：確認您的作業系統支援 POSIX 的 socket 函式庫（例如 `sys/socket.h`、`netinet/in.h`、`arpa/inet.h`）。
4. **要安裝的函式庫**：確認您的作業系統輸入過以下指令
    - sudo apt update
    - sudo apt install g++ make libssl-dev libsdl2-dev libsdl2-mixer-dev libsndfile1-dev

#### 函式庫
專案只需使用系統內建的標準 C++ 函式庫，因此無需安裝額外的函式庫。請確認環境支援以下標頭檔：
- `<iostream>`、`<string>`、`<cstring>`、`<vector>`、`<fstream>`、`<sstream>`（用於一般 I/O 和字串處理）。
- `<sys/socket.h>`、`<netinet/in.h>`、`<arpa/inet.h>`、`<netdb.h>`（用於 socket 程式設計和網路通訊）。

---

## 專案結構

專案的主要目錄結構如下：
```plaintext
41247066S/
├── README.md               # 文件說明
└── code/                   # 源碼目錄
    ├── Makefile            # 編譯指令
    ├── client.cpp          # 客戶端程式
    ├── server.cpp          # 伺服器端程式
    ├── media/              # 媒體相關檔案（目前未使用）
    ├── data/               # 儲存用戶資料和登入資訊
    │   ├── online_table.txt # 已登入但未登出的用戶資料
    │   └── users_table.txt  # 用戶註冊資料
    └── utils/              # 工具函數目錄
        ├── auth_utils.cpp  # 認證工具函數實作
        ├── auth_utils.h    # 認證工具函數標頭檔
        ├── message_utils.cpp # 訊息處理工具函數實作
        ├── message_utils.h # 訊息處理工具函數標頭檔
        ├── network_utils.cpp # 網絡工具函數實作
        └── network_utils.h # 網絡工具函數標頭檔
```
## 安裝與執行

### 安裝步驟

1. **下載源碼**：下載並解壓縮專案到您的工作目錄。
2. **編譯程式**：
   - 在 `code` 目錄下執行 `make` 命令，根據 Makefile 編譯 `server.cpp` 和 `client.cpp`：
     ```bash
     cd code
     make
     ```
   - 編譯完成後將生成 `server` 和 `client` 可執行檔。

### 執行伺服器和客戶端

1. **啟動伺服器**：
   - 使用 TCP 和 UDP 端口號來啟動伺服器（如果未指定端口，會使用預設的 8080 和 9090）：
     ```bash
     ./server [TCP_PORT] [UDP_PORT]
     ```
   - 例如，使用 8080 作為 TCP 端口，9090 作為 UDP 端口：
     ```bash
     ./server 8080 9090
     ```

2. **啟動客戶端**：
   - 指定伺服器的 IP（或主機名）和 TCP 端口來連接伺服器：
     ```bash
     ./client <hostname或IP> <port>
     ```
   - 例如，連接到本地伺服器的 8080 端口：
     ```bash
     ./client 127.0.0.1 8080
     ```

---
