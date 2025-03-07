# Soph_wireless

Soph wireless is a steamvr driver that registers and injects tracking information from another, remote steamvr instance.

For example, in a scenario with a laptop with attached tracker dongles in home 1, and a gaming computer in home 2.
The gaming computer in home 2 would start steamvr with this driver, and the laptop in home 1 would start the transmitter application alongside steamvr.
The laptop will now transmit tracker position, rotation, etc. to the gaming computer in home 2, making all those trackers available there.

## What is the use-case?

The usecase for which I built this is that, I want to be able to have full-body tracking while staying at other people's places that have base stations.
I wanted to be able to just plug my vive tracker dongles into my laptop, send them over the internet, and use them for vrchat.

## How does it work?

It works by listening on UDP port 6767, the related application, [soph_wireless_transmitter](https://github.com/SophiaH67/soph_wireless_transmitter)
will ask that steamvr instance for the position and rotation of connected trackers, and send them over to this driver.
This driver will see those packets, register the trackers if they haven't already been, and update their pose.
