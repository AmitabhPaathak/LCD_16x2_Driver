16X2 LCD DRIVER

It is a platform io project based on Arduino Mega with 16x2 LCD controller.It a project made on Platform IO which simulated 16x2 LCD using Arduino Mega mcu locally on my pc. You can pull this code in a Platform IO folder in your VS code and run it too. Before pulling this code ensure that you have configured the platform io extension for running Wokwi and have its license(free).

#NoArduinoLibrariesUsed I have used direct register programming to drive the 8x8 led matrix.


#SimulationProject

You can copy the diagram.json file from here and paste in Wokwi simulator for Arduino mega board and copyt the main.cpp file and run the simulation. Just add the while(1) loop inside void loop function as it(Wokwi/Arduino IDE) follows different programming style. The 'void setup()' function is nothing but the register's address declaration and the 'void loop()' function is nothing but a while loop.

You can simulate it in VS code with Platform IO as well as in Wokwi. The choice is yours.

Comments have been given to explain the logic as simply as possible.

Here are few videos of the simulation: