import { render, screen } from '@testing-library/react'
import userEvent from '@testing-library/user-event'
import { describe, expect, it, vi } from 'vitest'
import AuthForm from '../AuthForm'

describe('AuthForm', () => {
  it('renders callsign and passcode inputs with a login button by default', () => {
    render(<AuthForm onAuth={vi.fn()} />)

    expect(screen.getByPlaceholderText(/navigator_07/i)).toBeInTheDocument()
    expect(screen.getByPlaceholderText('••••••••')).toBeInTheDocument()
    expect(screen.getByRole('button', { name: /enter the maze/i })).toBeInTheDocument()
  })

  it('switches to register mode when the link is clicked', async () => {
    const user = userEvent.setup()
    render(<AuthForm onAuth={vi.fn()} />)

    await user.click(screen.getByRole('button', { name: /need a callsign\? register/i }))

    expect(screen.getByRole('button', { name: /create callsign/i })).toBeInTheDocument()
  })
})
