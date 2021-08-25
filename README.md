# Relaying-In-Modern-Power-System

## About the Project:
This project is based on creating a basic model of relaying of information in modern Smart Grids and power system. The information can consist of various domains like current through the power lines, or the real-time energy consumption by various loads, whether they are household or commercial. This model of smooth, fast & reliable communication is the way for improving the stability of the system. <ins> Electricity is a type of energy which is very difficult to store and hence reaching a dynamic equilibrium for production and consumption of electricity a must, in order to save electricity</ins>

## Benifits of the Project:
* Real-time monitoring of the Smart Grid and Power Systems.
* Fault Detection and then circuit protection in under 500 ms.
* Protection of our system from Electricity Theft, a problem  plagues our current Power Industry Infrastructure and causes billions of dollars loss annually.
* Real-time analysis of Energy consumption for each and individual consumers.

## Working of the Project:
The smooth and fast communication of the power system is obtained via the following steps, i.e., working of our model is as follows:
1.	The Arduinos connected across all the circuits, constantly measure the rms value of current currently passing through each phase, viz, Phase R, Phase Y, Phase B, and also through each consumer, i.e., Load1, Load2, Load3 which are connected to Phase R, Phase Y, Phase B respectively.
2.	The Arduino compares the value of current passing through each of the circuits with the maximum permissible current value for each of the circuit. (In our circuit it is 5A, as the transformers are rated at 5A Secondary). In case of overcurrent through any circuit, the Arduino sends a tripping signal to the circuit breakers (Single Channel relay module in our model), which is connected to the corresponding circuit. This isolates the faulty section and removes it from the grid, achieving best possible case of selectivity.   
3.	The Arduino calculates the amount of energy consumed by its corresponding section and relays this information to the node MCU connected to it, via serial communication. This relaying is only required in our model, as Arduino doesn’t have a Tcp/Ip stack for cloud computing.
4.	The NODE MCU is connected to a cloud database (here firebase real-time database). It updates the values of energy consumption by the consumer load and energy supplied by the distribution transformer in real-time.
5.	The energy supplied by distribution transformer and energy consumed by the loads is constantly monitored at the server-side. 
6.	In case of large disparity among the energy supplied and energy consumed, i.e., the case of ‘Electricity Theft’, the theft flags in the database are updated and are relayed to the NODE MCU, which in-turn, flags the tripping signals of the Arduino and the circuit breakers are tripped. 

The codes for the Arduino and NodeMCU are provided above.

