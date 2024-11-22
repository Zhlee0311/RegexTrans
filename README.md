# Regex Trans To FSM


## Function

1. **NFA**
<img src="./fig/NFA.png" alt="NFA"/>
<br/>

2. **DFA**
<img src="./fig/DFA.png" alt="DFA"/>
<br/>

3. **MDFA** (Minimal DFA)
<img src="./fig/MDFA.png" alt="MDFA"/>
<br/>



## Usage


1. **client**

```bash
cd client
npm install 
npm run dev
```

2. **server**

- [crow](https://crowcpp.org/master/)
- [asio](https://think-async.com/Asio/)
- [cmake](https://cmake.org/)
- [nlohmann json](https://github.com/nlohmann/json)

```bash
brew install crow asio cmake nlohmann-json
```

Ensure the include directory like this, then modify the [`CMakeLists.txt`](./server/CMakeLists.txt)
```
/usr/local/Homebrew/include/
├── asio
├── asio.hpp
├── crow
├── crow.h
├── nlohmann
├── ...
```

```bash
cd server
mkdir build && cd build
cmake ..
make -j4
./server
```

