import type { VoidProps } from "solid-js"
import { splitProps, type ComponentProps, type ValidComponent } from "solid-js"
import { Slider as SliderPrimitive } from "@kobalte/core/slider"

import { cx } from "@/lib/cva"

export type SliderProps<T extends ValidComponent = "div"> = ComponentProps<
  typeof SliderPrimitive<T>
>

export const Slider = <T extends ValidComponent = "div">(
  props: SliderProps<T>,
) => {
  const [, rest] = splitProps(props as SliderProps, ["class"])
  return (
    <SliderPrimitive
      data-slot="slider"
      class={cx(
        "relative flex w-full touch-none flex-col items-center gap-2 select-none data-[disabled]:opacity-50",
        props.class,
      )}
      {...rest}
    />
  )
}

export type SliderTrackProps<T extends ValidComponent = "div"> = ComponentProps<
  typeof SliderPrimitive.Track<T>
>

export const SliderTrack = <T extends ValidComponent = "div">(
  props: SliderTrackProps<T>,
) => {
  const [, rest] = splitProps(props as SliderTrackProps, ["class"])
  return (
    <SliderPrimitive.Track
      data-slot="slider-track"
      class={cx(
        "bg-muted relative h-1.5 w-full rounded-full",
        props.class,
      )}
      {...rest}
    />
  )
}

export type SliderFillProps<T extends ValidComponent = "div"> = VoidProps<
  ComponentProps<typeof SliderPrimitive.Fill<T>>
>

export const SliderFill = <T extends ValidComponent = "div">(
  props: SliderFillProps<T>,
) => {
  const [, rest] = splitProps(props as SliderFillProps, ["class"])
  return (
    <SliderPrimitive.Fill
      data-slot="slider-fill"
      class={cx("bg-primary absolute h-full rounded-full", props.class)}
      {...rest}
    />
  )
}

export type SliderThumbProps<T extends ValidComponent = "span"> = VoidProps<
  ComponentProps<typeof SliderPrimitive.Thumb<T>>
>

export const SliderThumb = <T extends ValidComponent = "span">(
  props: SliderThumbProps<T>,
) => {
  const [, rest] = splitProps(props as SliderThumbProps, ["class"])
  return (
    <SliderPrimitive.Thumb
      data-slot="slider-thumb"
      class={cx(
        "border-primary bg-background ring-ring/50 size-4 rounded-full border shadow-sm transition-[color,box-shadow] hover:ring-4 focus-visible:ring-4 focus-visible:outline-hidden disabled:pointer-events-none disabled:opacity-50 -top-1",
        props.class,
      )}
      {...rest}
    >
      <SliderPrimitive.Input />
    </SliderPrimitive.Thumb>
  )
}

export type SliderValueLabelProps<T extends ValidComponent = "div"> =
  ComponentProps<typeof SliderPrimitive.ValueLabel<T>>

export const SliderValueLabel = <T extends ValidComponent = "div">(
  props: SliderValueLabelProps<T>,
) => {
  const [, rest] = splitProps(props as SliderValueLabelProps, ["class"])
  return (
    <SliderPrimitive.ValueLabel
      data-slot="slider-value-label"
      class={cx("text-sm font-medium select-none", props.class)}
      {...rest}
    />
  )
}

export type SliderLabelProps<T extends ValidComponent = "label"> =
  ComponentProps<typeof SliderPrimitive.Label<T>>

export const SliderLabel = <T extends ValidComponent = "label">(
  props: SliderLabelProps<T>,
) => {
  const [, rest] = splitProps(props as SliderLabelProps, ["class"])
  return (
    <SliderPrimitive.Label
      data-slot="slider-label"
      class={cx("text-sm font-medium select-none", props.class)}
      {...rest}
    />
  )
}
