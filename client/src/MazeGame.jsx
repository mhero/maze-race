import { useCallback, useEffect, useRef, useState } from 'react'
import { WS_BASE } from './api'

const CELL = 26
const NORTH = 1
const EAST = 2
const SOUTH = 4
const WEST = 8

export default function MazeGame({ token, username, roomCode, onExit }) {
  const canvasRef = useRef(null)
  const wsRef = useRef(null)
  const trailsRef = useRef({}) // username -> [{row,col}, ...] recent trail

  const [maze, setMaze] = useState(null) // { size, walls }
  const [players, setPlayers] = useState({}) // username -> { username, row, col, color }
  const [status, setStatus] = useState('connecting')
  const [winner, setWinner] = useState(null)
  const [errorMsg, setErrorMsg] = useState(null)

  useEffect(() => {
    const ws = new WebSocket(
      `${WS_BASE}/ws/game?token=${encodeURIComponent(token)}&room=${encodeURIComponent(roomCode)}`
    )
    wsRef.current = ws

    ws.onopen = () => setStatus('connected')
    ws.onclose = () => setStatus('disconnected')
    ws.onerror = () => setStatus('error')

    ws.onmessage = (evt) => {
      const msg = JSON.parse(evt.data)

      if (msg.type === 'joined') {
        setMaze(msg.maze)
        const p = {}
        for (const pl of msg.players) p[pl.username] = pl
        p[msg.you.username] = msg.you
        setPlayers(p)
      } else if (msg.type === 'player_joined') {
        setPlayers((prev) => ({ ...prev, [msg.username]: msg }))
      } else if (msg.type === 'position') {
        setPlayers((prev) => {
          const existing = prev[msg.username] || {}
          const trail = trailsRef.current[msg.username] || []
          trailsRef.current[msg.username] = [...trail, { row: existing.row, col: existing.col }].slice(-6)
          return { ...prev, [msg.username]: { ...existing, row: msg.row, col: msg.col } }
        })
      } else if (msg.type === 'player_left') {
        setPlayers((prev) => {
          const next = { ...prev }
          delete next[msg.username]
          return next
        })
      } else if (msg.type === 'game_over') {
        setWinner(msg.winner)
      } else if (msg.type === 'error') {
        setErrorMsg(msg.message)
      }
    }

    return () => ws.close()
  }, [token, roomCode])

  const sendMove = useCallback(
    (dir) => {
      if (wsRef.current && wsRef.current.readyState === WebSocket.OPEN && !winner) {
        wsRef.current.send(JSON.stringify({ type: 'move', dir }))
      }
    },
    [winner]
  )

  useEffect(() => {
    const keyMap = {
      ArrowUp: 'up',
      ArrowDown: 'down',
      ArrowLeft: 'left',
      ArrowRight: 'right',
      w: 'up',
      s: 'down',
      a: 'left',
      d: 'right',
    }
    const handler = (e) => {
      const dir = keyMap[e.key]
      if (dir) {
        e.preventDefault()
        sendMove(dir)
      }
    }
    window.addEventListener('keydown', handler)
    return () => window.removeEventListener('keydown', handler)
  }, [sendMove])

  useEffect(() => {
    if (!maze || !canvasRef.current) return
    const canvas = canvasRef.current
    const ctx = canvas.getContext('2d')
    const size = maze.size
    canvas.width = size * CELL
    canvas.height = size * CELL

    ctx.clearRect(0, 0, canvas.width, canvas.height)
    ctx.fillStyle = '#0b1330'
    ctx.fillRect(0, 0, canvas.width, canvas.height)

    // dot-grid, blueprint-paper texture
    ctx.fillStyle = 'rgba(94, 234, 212, 0.08)'
    for (let r = 0; r < size; r++) {
      for (let c = 0; c < size; c++) {
        ctx.beginPath()
        ctx.arc(c * CELL + 1, r * CELL + 1, 1, 0, Math.PI * 2)
        ctx.fill()
      }
    }

    // goal cell
    const center = Math.floor(size / 2)
    ctx.fillStyle = 'rgba(251, 191, 36, 0.22)'
    ctx.fillRect(center * CELL, center * CELL, CELL, CELL)
    ctx.strokeStyle = 'rgba(251, 191, 36, 0.9)'
    ctx.lineWidth = 1.5
    ctx.strokeRect(center * CELL + 3, center * CELL + 3, CELL - 6, CELL - 6)

    // walls
    ctx.strokeStyle = '#5eead4'
    ctx.lineWidth = 2
    ctx.lineCap = 'round'
    ctx.shadowColor = 'rgba(94, 234, 212, 0.9)'
    ctx.shadowBlur = 5

    for (let row = 0; row < size; row++) {
      for (let col = 0; col < size; col++) {
        const walls = maze.walls[row * size + col]
        const x = col * CELL
        const y = row * CELL
        ctx.beginPath()
        if (walls & NORTH) {
          ctx.moveTo(x, y)
          ctx.lineTo(x + CELL, y)
        }
        if (walls & WEST) {
          ctx.moveTo(x, y)
          ctx.lineTo(x, y + CELL)
        }
        if (walls & EAST) {
          ctx.moveTo(x + CELL, y)
          ctx.lineTo(x + CELL, y + CELL)
        }
        if (walls & SOUTH) {
          ctx.moveTo(x, y + CELL)
          ctx.lineTo(x + CELL, y + CELL)
        }
        ctx.stroke()
      }
    }

    // player trails
    ctx.shadowBlur = 0
    for (const p of Object.values(players)) {
      const trail = trailsRef.current[p.username] || []
      trail.forEach((t, i) => {
        if (t.row == null) return
        ctx.beginPath()
        ctx.fillStyle = p.color
        ctx.globalAlpha = 0.08 * (i + 1)
        ctx.arc(t.col * CELL + CELL / 2, t.row * CELL + CELL / 2, CELL * 0.14, 0, Math.PI * 2)
        ctx.fill()
      })
    }
    ctx.globalAlpha = 1

    // players
    ctx.shadowBlur = 14
    for (const p of Object.values(players)) {
      ctx.beginPath()
      ctx.fillStyle = p.color
      ctx.shadowColor = p.color
      const cx = p.col * CELL + CELL / 2
      const cy = p.row * CELL + CELL / 2
      ctx.arc(cx, cy, CELL * 0.3, 0, Math.PI * 2)
      ctx.fill()
    }
    ctx.shadowBlur = 0
  }, [maze, players])

  return (
    <div className="screen game-screen">
      <div className="game-topbar">
        <div className="eyebrow">MAZE // RACE</div>
        <div className="room-badge">
          ROOM <strong>{roomCode}</strong>
        </div>
        <div className={`status-pill status-${status}`}>{status}</div>
        <button className="link-button" onClick={onExit}>
          leave
        </button>
      </div>

      {errorMsg && <div className="error-banner">{errorMsg}</div>}

      <div className="game-body">
        <div className="canvas-frame">
          <canvas ref={canvasRef} className="maze-canvas" />
        </div>

        <div className="side-panel">
          <h3 className="section-title">Racers</h3>
          <ul className="players-list">
            {Object.values(players).map((p) => (
              <li key={p.username}>
                <span className="dot" style={{ background: p.color, boxShadow: `0 0 8px ${p.color}` }} />
                {p.username}
                {p.username === username ? ' (you)' : ''}
              </li>
            ))}
          </ul>
          <p className="hint">Arrow keys or WASD to move. Reach the glowing amber cell first.</p>
        </div>
      </div>

      {winner && (
        <div className="winner-banner">
          🏁 {winner === username ? 'You reached the center first!' : `${winner} reached the center first.`}
        </div>
      )}
    </div>
  )
}
