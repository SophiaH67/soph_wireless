# Soph_wireless

Soph wireless is a steamvr driver that registers and injects tracking information for packets it receives from https://github.com/SophiaH67/soph_wireless_transmitter

## What is the usecase?

The usecase for which I built this is that, I want to be able to have full-body tracking while staying at other people's places that have base stations.
I wanted to be able to just plug my vive tracker dongles into my laptop, send them over the internet, and use them for vrchat.

## How does it work?

It works by listening on UDP port 6767, the related application, [soph_wireless_transmitter](https://github.com/SophiaH67/soph_wireless_transmitter)
will ask that steamvr instance for the position and rotation of connected trackers, and send them over to this driver.
This driver will see those packets, register the trackers if they haven't already been, and update their pose.
