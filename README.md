# Deadman Land (failsafe) application for Crazyflie 2.x

This repos contains the deadman land application for Cazyflie 2.x.
It uses out-of-tree build functionality of the Crazyflie firmware and is implemented using the app entry-point.

This application requires a Crazyflie 2.x with Flow deck version 2.0 and Multiranger deck attached. If you want to use the Flow deck 1.0 or any other positioning deck, you will need to change the following line to look at the parameter for the wanted deck:
```
  uint16_t idPositioningDeck = paramGetVarId("deck", "bcFlow2");

```

When running, if the connection to the Crazyradio PA is interrupted the 
application will land the Crazyflie 2.x 

## Build

You must have the required tools to build the [Crazyflie firmware](https://github.com/bitcraze/crazyflie-firmware).

Clone the repos with ```--recursive```. If you did not do so, pull submodules with:
```
git submodule update --init --recursive
```

Clone this repository to this location in Bitcraze VM:
```
~/crazyflie-firmware/examples/app_deadman_land
```

Navigate to the new directory to build and bootload:
```
make -j$(nproc)
make cload
```


