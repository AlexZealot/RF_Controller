#ifndef __BUTTON__
#define __BUTTON__

#define UPD_INTERVAL  100

class Button {
  private:
    unsigned long lastTick;
    unsigned long whenPressed;
    int           PIN;
    int           downLeft;

    bool          state;
    bool          lastState;
  public:
    Button(int _pin);

    void          update(unsigned long T);
    bool          isDown();
    bool          isPressed();
    bool          isReleased();
    unsigned long holdInterval(unsigned long T);
};

#endif //__BUTTON__
