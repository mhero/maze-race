import { useState } from 'react'
import AuthForm from './AuthForm.jsx'
import Lobby from './Lobby.jsx'
import MazeGame from './MazeGame.jsx'

export default function App() {
  const [auth, setAuth] = useState(null) // { token, username }
  const [roomCode, setRoomCode] = useState(null)

  if (!auth) {
    return <AuthForm onAuth={setAuth} />
  }

  if (!roomCode) {
    return (
      <Lobby
        token={auth.token}
        username={auth.username}
        onJoin={setRoomCode}
        onLogout={() => setAuth(null)}
      />
    )
  }

  return (
    <MazeGame
      token={auth.token}
      username={auth.username}
      roomCode={roomCode}
      onExit={() => setRoomCode(null)}
    />
  )
}
