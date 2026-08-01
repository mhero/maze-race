import { describe, expect, it } from 'vitest'
import { keyToDirection } from '../keyboard'

describe('keyToDirection', () => {
  it('maps arrow keys to directions', () => {
    expect(keyToDirection('ArrowUp')).toBe('up')
    expect(keyToDirection('ArrowDown')).toBe('down')
    expect(keyToDirection('ArrowLeft')).toBe('left')
    expect(keyToDirection('ArrowRight')).toBe('right')
  })

  it('maps WASD keys to directions', () => {
    expect(keyToDirection('w')).toBe('up')
    expect(keyToDirection('s')).toBe('down')
    expect(keyToDirection('a')).toBe('left')
    expect(keyToDirection('d')).toBe('right')
  })

  it('returns undefined for unrelated keys', () => {
    expect(keyToDirection('Enter')).toBeUndefined()
    expect(keyToDirection('W')).toBeUndefined() // case-sensitive on purpose
    expect(keyToDirection(' ')).toBeUndefined()
  })
})
