declare module 'spiky-nes' {

    export const SCREEN_WIDTH: number
    export const SCREEN_HEIGHT: number

    export interface ButtonEnum {
        value: number
    }

    export interface ButtonConstructor {
        A: ButtonEnum
        B: ButtonEnum
        SELECT: ButtonEnum
        START: ButtonEnum
        UP: ButtonEnum
        DOWN: ButtonEnum
        LEFT: ButtonEnum
        RIGHT: ButtonEnum
    }

    export const Button: ButtonConstructor

    export class CPURegisters {
        A: number
        X: number
        Y: number
        P: number
        SP: number
        PC: number
    }

    export class Emulator {
        constructor()
        loadROM(data: Uint8Array | number[]): boolean
        reset(): void
        step(): number
        execOp(): number
        run(milliseconds: number): void
        runFrame(sampleRate?: number): Float32Array
        frameBuffer(): Uint8ClampedArray
        audioSample(): number
        press(button: ButtonEnum, player?: number): void
        release(button: ButtonEnum, player?: number): void
        cpuRegisters(): CPURegisters
        cpuRead(addr: number): number
        cpuWrite(addr: number, value: number): void
        cpuCycles(): number
        frame(): number
        halted(): boolean
        mapper(): number
        delete(): void
    }

}
