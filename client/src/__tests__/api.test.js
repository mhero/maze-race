import { describe, expect, it } from 'vitest'
import { handle } from '../api'

function fakeResponse(ok, status, body) {
  return {
    ok,
    status,
    json: () => Promise.resolve(body),
  }
}

describe('handle', () => {
  it('resolves with the parsed body on a successful response', async () => {
    const data = await handle(fakeResponse(true, 200, { token: 'abc', username: 'alice' }))
    expect(data).toEqual({ token: 'abc', username: 'alice' })
  })

  it('throws the server-provided error message on failure', async () => {
    await expect(handle(fakeResponse(false, 401, { error: 'invalid username or password' }))).rejects.toThrow(
      'invalid username or password'
    )
  })

  it('falls back to a generic message when the failed response has no error field', async () => {
    await expect(handle(fakeResponse(false, 500, {}))).rejects.toThrow('Request failed (500)')
  })

  it('falls back to a generic message when the body is not valid JSON', async () => {
    const res = {
      ok: false,
      status: 500,
      json: () => Promise.reject(new Error('not json')),
    }
    await expect(handle(res)).rejects.toThrow('Request failed (500)')
  })
})
