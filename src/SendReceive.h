#ifndef SENDRECEIVE_H
#define SENDRECEIVE_H

#include <Arduino.h>

#define ALLOW_SEND_WITHOUT_RECEIVE true  //enable this to allow sending commands without having first received any
#define IGNORE_INVALID_START_PULSE false //enable this to allow testing without a valid start pulse

#define WAIT_AFTER_RECEIVE_COMMAND 10 //milliseconds to wait after a command has been received before sending a command, the window is ~40ms
#define WAIT_AFTER_SEND_COMMANDS 100  //millisconds to wait after sending a command before sending another command

//Timings in us
#define MAX_MESSAGE_LEN 10300

#define START_TOLERANCE 100
#define BIT_TOLERANCE 10
#define BIT_TIME_ADJUST 3
#define BUTTON_TIME 147
#define BIT_TIME 440
#define SHORT_BIT_TIME 330

#define DATA_START_LEN 4440 //length of the start pulse in us
#define DATA_START_LEN_MIN DATA_START_LEN - START_TOLERANCE
#define DATA_START_LEN_MAX DATA_START_LEN + START_TOLERANCE

#define START_PULSE_COUNT 440
#define START_SPACE_COUNT 18

#define BIT_COUNT 13 //number of bits to read in
#define DATA_MASK 1 << (BIT_COUNT - 1)

#define MAX_OUT_COMMANDS 20 //maximum number of commands to keep in the outgoing queue

/*
 * RM_WAIT_PULSE, start pulse received -> RM_WAIT_PULSE_END, timer elapses -> RM_WAIT_DATA_END, 13 bits clocked in -> RM_WAIT_PULSE
 */
#define RM_WAIT_PULSE 0     //waiting for the start pulse to go high
#define RM_WAIT_PULSE_END 1 //waiting for the start pulse to go low
#define RM_WAIT_DATA_END 2  //waiting for the data to end

/*
 * SM_WAIT, data in out buffer -> SM_START_END, timer elapses -> SM_GAP_END, timer elapses -> SM_BUTTON_END, 13 bits clocked out -> SM_WAIT
 */
#define SM_WAIT 0
#define SM_SENDING 1

class SendReceive
{
public:
  SendReceive(int dataInPin, int dataOutPin, int debugPin);
  void setup(void (&onSetDataInterrupt)(bool state), void (&onDebug)(char *message), void (&onDebugf)(char *fmt, ...));
  void loop();

  virtual void onCommandReceived(unsigned int command) = 0;
  virtual void onCommandSent(unsigned int command) = 0;

  void ICACHE_RAM_ATTR dataInterrupt();

  unsigned long getLastCommandSentTime();
  unsigned long getLastCommandQueuedTime();
  unsigned long getLastCommandReceivedTime();

  bool queueCommand(unsigned int command);
  bool queueCommands(unsigned int command, int count);
  int getCommandQueueCount();
  void printMessageData(bool includeBreakdown);

private:
  int dataInPin;
  int dataInPinBit;
  int dataEnablePin;
  int dataEnablePinBit;
  int debugPin;
  int debugPinBit;

  void (*setDataInterrupt)(bool state);
  void (*debug)(char *message);
  void (*debugf)(char *fmt, ...);

  int bitIndex;
  //int bitCount;
  unsigned int times[16];
  bool states[16];

  int dataIndex = 0;
  unsigned long dataStart = 0;
  unsigned long lastBitStartTime = 0;

  int getSendBitTime(int bitPos);

  unsigned long startPulseLength;
  unsigned long startPulseStartTime;
  bool invalidStartPulse;
  volatile unsigned int inBitPos;
  volatile unsigned int inData;
  volatile int RECV_MODE;

  unsigned int commandQueue[MAX_OUT_COMMANDS];
  int commandQueueCount;

  volatile unsigned int receivedCommand;

  unsigned int commandToSend;
  unsigned int outBitPos;
  int SEND_MODE;

  unsigned long lastCommandSentTime;
  unsigned long lastCommandQueuedTime;
  unsigned long lastCommandReceivedTime;

  void processOutgoingCommandQueue();
  void processIncomingCommand();
  void sendCommand(unsigned int command);

  unsigned long now;
  unsigned long bitTime;
  bool nonStartBit;
  bool validStartBit;
  bool bitState;
  bool lastBitState;

  void printCommandQueue();
  int getReceiveBitTime(int bitIndex);
  unsigned int decode(unsigned int times[], bool states[]);
};

#endif
