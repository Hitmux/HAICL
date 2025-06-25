# HAICL (Hitmux AI in Command Line)

HAICL是一个命令行工具，旨在提供与AI模型（如OpenAI和Google AI）交互的统一接口。它支持快速提问、多轮对话、历史记录管理和灵活的配置。

## 功能特性

*   **统一API接口：** 兼容OpenAI和Google的模型，未来易于扩展。
*   **命令行调用：** 提供简洁的命令行模式，支持快速提问。
*   **交互模式：** 提供友好的多轮对话体验，支持上下文管理。
*   **历史记录：** 自动或手动保存聊天记录（txt格式），支持加载、继续对话、和修改历史提问。
*   **配置灵活：** API密钥和URL通过环境变量读取；支持配置文件（如JSON）进行高级设置（模型参数、代理等）。

## 构建和运行

### 前置条件

*   C++17 兼容的编译器 (e.g., g++)
*   CMake (>= 3.10)
*   libcurl 开发库

### 构建步骤

1.  进入项目根目录：
    ```bash
    cd HAICL
    ```
2.  创建构建目录并配置CMake：
    ```bash
    mkdir build
    cd build
    cmake ..
    ```
3.  编译项目：
    ```bash
    make
    ```

### 运行

编译成功后，可执行文件 `haicl` 将位于 `build/` 目录下。

#### 快速提问模式

```bash
./build/haicl -p "你的问题" -t <模型类型> -m <模型名称> [--param key=value]
```

示例：
```bash
# 设置环境变量 (推荐)
export OPENAI_API_KEY="YOUR_API_KEY"
export OPENAI_BASE_URL="https://api.deepseek.com/v1"

./build/haicl -p "你好，世界！" -t openai -m deepseek-chat --param temperature=0.7
```

#### 交互模式

```bash
./build/haicl -i [-t <模型类型>] [-m <模型名称>] [--load-history <文件名>]
```

在交互模式下，你可以使用以下命令：

*   `exit`: 退出交互模式。
*   `save`: 保存当前对话到新的历史文件。
*   `list`: 列出所有已保存的历史文件。
*   `show`: 显示当前对话内容。
*   `load <文件名>`: 从指定文件加载对话历史。
*   `modify <索引> <新内容>`: 修改当前对话中指定索引的消息。
*   `help`: 显示命令帮助。

## 配置

HAICL会从以下位置按优先级加载配置（优先级从高到低）：

1.  命令行参数
2.  环境变量 (e.g., `OPENAI_API_KEY`, `GOOGLE_API_KEY`, `DEFAULT_AI_MODEL`)
3.  `config.json` 配置文件

`config.json` 示例：

```json
{
    "default_ai_model": "openai",
    "openai": {
        "api_key": "sk-from-config",
        "base_url": "https://api.openai.com/v1-from-config",
        "model_name": "gpt-4-from-config"
    },
    "google": {
        "api_key": "google-key-from-config",
        "base_url": "https://generativelanguage.googleapis.com-from-config",
        "model_name": "gemini-pro-from-config"
    }
}
```

## 开源

本项目遵循MIT许可证。欢迎贡献！


