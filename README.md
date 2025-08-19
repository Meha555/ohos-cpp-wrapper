# ohos-cpp-wrapper

汇总我写的HarmonyOS C++ Wrapper，部分项目没有维护单独的工程。

- commev：封装 CommonEvent 模块
- device：封装 DDK 模块，目前仅有USB（设计的不是很合理，我不了解USB）
- logging: 封装OH_Log_Print为 `std::ostream` 的单例对象，使其支持使用 `std::ostream` 作为输出流的库

## TODO

- [ ] 添加示例工程
