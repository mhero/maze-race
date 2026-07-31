# Maze Race 🧩

A real-time multiplayer maze race: up to 4 players drop into the same
procedurally generated maze and race to be first to reach the center cell.
Built to showcase a modern **C++ / Drogon** backend (REST + WebSockets, JWT
auth, SQLite) paired with a small **React** client.

```
 ┌───────────────────────────────┐        WebSocket (live positions)
 │   React client (Vite)         │◄──────────────────────────────────┐
 │   localhost:5173              │                                   │
 └───────────────┬───────────────┘                                   │
                  │ REST (register / login / rooms / leaderboard)    │
                  ▼                                                  │
 ┌───────────────────────────────┐                                   │
 │   Drogon C++ server            │──────────────────────────────────┘
 │   localhost:5555               │
 │   controllers/ filters/        │
 │   services/ (JWT, maze gen,    │
 │   room manager, SQLite)        │
 └───────────────────────────────┘
```

## What it demonstrates

- Drogon's standard project layout (`controllers/`, `filters/`, `config.json`,
  CMake + `find_package(Drogon CONFIG REQUIRED)`)
- A `WebSocketController` driving real-time, server-authoritative game state
  (movement is validated against the maze on the server, not trusted from
  the client)
- A `HttpFilter` doing JWT auth for protected REST routes
- Async SQLite access via Drogon's `DbClient` (`execSqlAsync`)
- A dependency-free HS256 JWT implementation and salted SHA-256 password
  hashing, both using only OpenSSL (already a Drogon dependency)
- A small, framework-free-ish React client that renders the maze on
  `<canvas>` and drives movement over a raw `WebSocket`

## Project structure

```
maze-race/
├── server/                  # C++ / Drogon backend
│   ├── CMakeLists.txt
│   ├── config.json
│   ├── main.cc
│   ├── controllers/
│   │   ├── AuthController.{h,cc}         POST /api/register, /api/login
│   │   ├── RoomController.{h,cc}         POST /api/rooms, GET /api/rooms/{code}
│   │   ├── LeaderboardController.{h,cc}  GET /api/leaderboard
│   │   └── GameController.{h,cc}         WS /ws/game
│   ├── filters/
│   │   └── JwtAuthFilter.{h,cc}
│   └── services/
│       ├── JwtService.{h,cc}
│       ├── PasswordService.{h,cc}
│       ├── MazeGenerator.h
│       ├── GameRoomManager.{h,cc}
│       └── LeaderboardService.{h,cc}
└── client/                  # React (Vite) frontend
    └── src/
        ├── App.jsx, AuthForm.jsx, Lobby.jsx, MazeGame.jsx
        ├── api.js
        └── styles.css
```

## 1. Prerequisites (macOS)

```bash
# Xcode command line tools (compiler, etc.)
xcode-select --install

# Homebrew, if you don't already have it: https://brew.sh

# Drogon + its usual deps
brew install drogon cmake openssl node
```

`brew install drogon` pulls in jsoncpp, trantor, sqlite3, and OpenSSL for
you. If the formula isn't available on your system (Homebrew occasionally
reshuffles formulas), build Drogon from source instead:

```bash
brew install jsoncpp uuid openssl@3 sqlite3 cmake
git clone https://github.com/drogonframework/drogon
cd drogon && git submodule update --init
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
sudo make install
```

## 2. Build & run the server

```bash
cd server
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
./maze_race_server
```

You should see `Maze Race server starting on port 5555`. A `maze.db` SQLite
file is created automatically next to the binary on first run (tables are
created if missing).

By default it listens on `http://localhost:5555` for REST and
`ws://localhost:5555/ws/game` for the game socket. Change the port or the
JWT secret in `server/config.json` (`listeners` / `custom_config.jwt_secret`)
if needed — do change the secret before deploying this anywhere real.

## 3. Run the React client

In a second terminal:

```bash
cd client
npm install
npm run dev
```

Open the URL Vite prints (typically `http://localhost:5173`).

## 4. Play

1. Register a callsign (username + password) — this hits `POST
   /api/register` and stores a JWT client-side.
2. **Create a room** to get a 6-character room code, or **join** one a
   friend created.
3. Open the same room code in up to 3 more browser tabs/devices (each with
   its own login) to race together.
4. Use arrow keys or WASD to move. First to reach the glowing amber cell in
   the center wins — the win is recorded and shows up on the lobby
   leaderboard.

## API summary

| Method | Path                | Auth | Description                          |
|--------|----------------------|------|---------------------------------------|
| POST   | `/api/register`      | —    | Create an account, returns a JWT      |
| POST   | `/api/login`         | —    | Log in, returns a JWT                 |
| POST   | `/api/rooms`         | JWT  | Create a room, returns its code       |
| GET    | `/api/rooms/{code}`  | —    | Basic room status                     |
| GET    | `/api/leaderboard`   | —    | Top winners by win count              |
| WS     | `/ws/game?token=&room=` | JWT (query param) | Join a room's live game session |

## Notes & limitations (by design, for a showcase project)

- Game rooms live in memory and are lost on server restart — there's no
  need for persistence beyond the leaderboard for this demo.
- Password hashing is salted SHA-256 for zero extra dependencies; use
  bcrypt/argon2 in anything real.
- CORS is wide open (`*`) to make local development trivial; lock this down
  for any real deployment.
- Mazes are fixed at odd sizes (default 15×15) so there's always a single,
  well-defined center cell.

## License

MIT — see [LICENSE](LICENSE).
