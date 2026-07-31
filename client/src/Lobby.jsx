import { useEffect, useState } from 'react'
import { createRoom, getLeaderboard } from './api'

export default function Lobby({ token, username, onJoin, onLogout }) {
  const [code, setCode] = useState('')
  const [error, setError] = useState(null)
  const [creating, setCreating] = useState(false)
  const [leaderboard, setLeaderboard] = useState([])
  const [leaderboardError, setLeaderboardError] = useState(false)

  useEffect(() => {
    getLeaderboard()
      .then(setLeaderboard)
      .catch(() => setLeaderboardError(true))
  }, [])

  const handleCreate = async () => {
    setError(null)
    setCreating(true)
    try {
      const room = await createRoom(token)
      onJoin(room.code)
    } catch (err) {
      setError(err.message)
    } finally {
      setCreating(false)
    }
  }

  const handleJoin = (e) => {
    e.preventDefault()
    const trimmed = code.trim().toUpperCase()
    if (trimmed) onJoin(trimmed)
  }

  return (
    <div className="screen lobby-screen">
      <div className="lobby-topbar">
        <div className="eyebrow">MAZE // RACE</div>
        <div className="whoami">
          <span>{username}</span>
          <button className="link-button" onClick={onLogout}>log out</button>
        </div>
      </div>

      <div className="lobby-grid">
        <div className="blueprint-card">
          <h2 className="section-title">Start a run</h2>
          <p className="subtitle">Spin up a new 15×15 maze and share the code with up to 3 friends.</p>
          <button className="btn-primary" onClick={handleCreate} disabled={creating}>
            {creating ? 'Plotting maze…' : 'Create new room'}
          </button>

          <div className="divider">
            <span>or</span>
          </div>

          <form onSubmit={handleJoin} className="join-form">
            <input
              placeholder="ROOM CODE"
              value={code}
              onChange={(e) => setCode(e.target.value.toUpperCase())}
              maxLength={6}
              className="code-input"
            />
            <button type="submit" className="btn-secondary">Join</button>
          </form>

          {error && <div className="error-banner">{error}</div>}
        </div>

        <div className="blueprint-card">
          <h2 className="section-title">Leaderboard</h2>
          {leaderboardError && <p className="subtitle">Couldn't reach the server for standings.</p>}
          {!leaderboardError && leaderboard.length === 0 && (
            <p className="subtitle">No finished races yet — be the first to reach center.</p>
          )}
          {leaderboard.length > 0 && (
            <ol className="leaderboard-list">
              {leaderboard.map((row, i) => (
                <li key={row.username}>
                  <span className="rank">{String(i + 1).padStart(2, '0')}</span>
                  <span className="lb-name">{row.username}</span>
                  <span className="lb-wins">{row.wins} win{row.wins === 1 ? '' : 's'}</span>
                </li>
              ))}
            </ol>
          )}
        </div>
      </div>
    </div>
  )
}
