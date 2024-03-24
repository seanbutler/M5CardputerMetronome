  
#if defined ( ARDUINO )

#include <Arduino.h>

// If you use SD card, write this.
// #include <SD.h>

// If you use SPIFFS, write this.
// #include <SPIFFS.h>

#endif

// * The filesystem header must be included before the display library.

//----------------------------------------------------------------

// If you use ATOM Display, write this.
#include <M5AtomDisplay.h>

// If you use Module Display, write this.
#include <M5ModuleDisplay.h>

// If you use Module RCA, write this.
#include <M5ModuleRCA.h>

// If you use Unit GLASS, write this.
#include <M5UnitGLASS.h>

// If you use Unit GLASS2, write this.
#include <M5UnitGLASS2.h>

// If you use Unit OLED, write this.
#include <M5UnitOLED.h>

// If you use Unit Mini OLED, write this.
#include <M5UnitMiniOLED.h>

// If you use Unit LCD, write this.
#include <M5UnitLCD.h>

// If you use UnitRCA (for Video output), write this.
#include <M5UnitRCA.h>

// * The display header must be included before the M5Unified library.

//----------------------------------------------------------------

// Include this to enable the M5 global instance.

#include <M5Unified.h>
#include "M5Cardputer.h"
#include "M5GFX.h"

#include "glib.hpp"
#include "lgfx/v1/lgfx_fonts.hpp"

// ============================================================

namespace time_sig {

  constexpr unsigned int max_bpm = 250;
  constexpr unsigned int min_bpm = 30;  
  int bpm;  

  void increase_bpm() {
    if ( bpm < max_bpm ) {
      bpm++;
    }
  }  

  void decrease_bpm() {
    if ( bpm > min_bpm ) {
      bpm--;
    }
  }

  constexpr unsigned int max_sig = 32;
  constexpr unsigned int min_sig = 2;

  int sig;

  void increase_sig() {
    if ( sig < max_sig ) {
      bpm++;
    }
  }  

  void decrease_sig() {
    if ( sig > min_sig ) {
      bpm--;
    }
  }
}

// ----------------------------------------------------------------------

namespace fsm {

  class machine;
  class state;

  class state {
    public:
      virtual void Enter(machine * machine) = 0;
      virtual void Execute(machine* context) = 0;
      virtual void Exit(machine * machine) = 0;
  };

  class machine {
    public:
      machine()
      : current_state_(0l)
      , previous_state_(0l) 
      {
      }
      
      state * current_state_;
      state * previous_state_;

      void Execute(){
          if ( previous_state_ != current_state_ )
          {
            previous_state_ = current_state_;
            current_state_->Enter(this);
          }
          else
          {
            current_state_->Execute(this);
          }
      }

      void SetCurrentState(state * new_state_){
          current_state_ = new_state_;
      }

      void ChangeState(state * new_state_){
          current_state_->Exit(this);

          previous_state_ = current_state_;
          current_state_ = new_state_;
      }
  };

}

// ----------------------------------------------------------------------

namespace app {

  namespace states {

    class intro_screen : public fsm::state {

      public:
        sb::Label title_label;
        sb::Label instruction_label;

        intro_screen() {
          Serial.printf("intro_screen ctor\r\n");
        }

        void Enter(fsm::machine * machine){
          Serial.printf("intro_screen Enter()\r\n");

          title_label.SetColour(YELLOW);
          title_label.SetText(String("Metronome!"));
          title_label.SetFont(fonts::FreeSans18pt7b);
          title_label.SetAlignment(middle_center);
          title_label.SetPosition(sb::Position(M5Cardputer.Display.width()/2, M5Cardputer.Display.height()/2));

          instruction_label.SetColour(YELLOW);
          instruction_label.SetText(String("Press Any Key"));
          instruction_label.SetFont(fonts::FreeSans9pt7b);
          instruction_label.SetAlignment(bottom_center);
          instruction_label.SetPosition(sb::Position(M5Cardputer.Display.width()/2, M5Cardputer.Display.height()-4));

          title_label.Render();
          instruction_label.Render();
        }

        void Execute(fsm::machine * machine);

        void Exit(fsm::machine * machine){
          Serial.printf("intro_screen Exit()\r\n");
          M5Cardputer.Display.clearDisplay();

          time_sig::bpm = 120;
          time_sig::sig = 4;
        }
    };

    class menu_screen : public fsm::state {
      public:
        sb::Label title_label;
        sb::Label bpm_label;
        sb::Label bpm_value_label;
        sb::Label sig_label;
        sb::Label sig_value_label;
        sb::Label instruction_label;


        menu_screen() {
          Serial.printf("menu_screen ctor\r\n");
        }

        void Render_Widgets() {

          title_label.SetText(String("Choose Your Quiz!"));
          title_label.SetColour(YELLOW);
          title_label.SetFont(fonts::FreeSans9pt7b);
          title_label.SetAlignment(top_center);
          title_label.SetPosition(sb::Position(M5Cardputer.Display.width()/2, 4));

          bpm_label.SetColour(RED);
          bpm_label.SetText(String("BPM: "));
          bpm_label.SetFont(fonts::FreeSans18pt7b);
          bpm_label.SetAlignment(middle_left);
          bpm_label.SetPosition(sb::Position(24,  (M5Cardputer.Display.height()/2)-22));

          bpm_value_label.SetColour(RED);
          bpm_value_label.SetText(String(time_sig::bpm));
          bpm_value_label.SetFont(fonts::FreeSans18pt7b);
          bpm_value_label.SetAlignment(middle_right);
          bpm_value_label.SetPosition(sb::Position(M5Cardputer.Display.width()-22, (M5Cardputer.Display.height()/2)-22));

          sig_label.SetColour(RED);
          sig_label.SetText(String("Signature: "));
          sig_label.SetFont(fonts::FreeSans18pt7b);
          sig_label.SetAlignment(middle_left);
          sig_label.SetPosition(sb::Position(24,  (M5Cardputer.Display.height()/2)+22));

          sig_value_label.SetColour(RED);
          sig_value_label.SetText(String(time_sig::sig));
          sig_value_label.SetFont(fonts::FreeSans18pt7b);
          sig_value_label.SetAlignment(middle_right);
          sig_value_label.SetPosition(sb::Position(M5Cardputer.Display.width()-24, (M5Cardputer.Display.height()/2)+22));

          instruction_label.SetColour(YELLOW);
          instruction_label.SetText(String("Choose U/D Change R/L"));
          instruction_label.SetFont(fonts::FreeSans9pt7b);
          instruction_label.SetAlignment(bottom_center);
          instruction_label.SetPosition(sb::Position(M5Cardputer.Display.width()/2, M5Cardputer.Display.height()-4));

          title_label.Render();
          bpm_label.Render();
          bpm_value_label.Render();
          sig_label.Render();
          sig_value_label.Render();
          instruction_label.Render();
        }

        void Enter(fsm::machine * machine){
          Serial.printf("menu_screen Enter()\r\n");
          Render_Widgets();
        }

        void Execute(fsm::machine * machine);

        void Exit(fsm::machine * machine){
          Serial.printf("intro_screen Exit()\r\n");
          M5Cardputer.Display.clearDisplay();
        }
    };

  // ----------------------------------------------------------------------

    intro_screen          intro_screen_state;
    menu_screen           menu_screen_state;

  // ----------------------------------------------------------------------

    void intro_screen::Execute(fsm::machine * machine){
      // Serial.printf("intro_screen Execute()\r\n");

      // listen for key presses
      if ( M5Cardputer.Keyboard.isChange()) {
        if ( M5Cardputer.Keyboard.isPressed()) {
          // change state to next...
          machine->ChangeState(&menu_screen_state );
        }
      }
    } 

    void menu_screen::Execute(fsm::machine * machine){
      Serial.printf("menu_screen Execute()\r\n");

      // listen for key presses              
      // if ( M5Cardputer.Keyboard.isChange()) {
        if ( M5Cardputer.Keyboard.isPressed()) 
        {
          
          if ( M5Cardputer.Keyboard.isKeyPressed(';') )
          {
            time_sig::increase_bpm();
          }

          if ( M5Cardputer.Keyboard.isKeyPressed('.') )
          {
            time_sig::decrease_bpm();
          }

          if ( M5Cardputer.Keyboard.isKeyPressed('/') )
          {
            time_sig::increase_sig();
          }

          if ( M5Cardputer.Keyboard.isKeyPressed(',') )
          {
            time_sig::decrease_sig();
          }

          M5Cardputer.Display.clearDisplay();
          Render_Widgets();
        }

      // }

    }

  // ----------------------------------------------------------------------

    class beep_state_t : public fsm::state {

      public:

        beep_state_t() {
          Serial.printf("beep_state_t ctor\r\n");
        }

        void Enter(fsm::machine * machine){

        }

        void Execute(fsm::machine * machine);

        void Exit(fsm::machine * machine)
        {
        }
    };


    class delay_state_t : public fsm::state {

      public:

        delay_state_t() {
          Serial.printf("delay_state_t ctor\r\n");
        }

        void Enter(fsm::machine * machine){

        }

        void Execute(fsm::machine * machine);

        void Exit(fsm::machine * machine)
        {
        }
    };

  }

}

// ============================================================

fsm::machine app_logic;

app::states::intro_screen intro_screen_state;
app::states::menu_screen menu_screen_state;


fsm::machine tone_logic;

app::states::beep_state_t beep_state;
app::states::delay_state_t delay_state;


void setup(void)
{
    M5Cardputer.begin();
    M5Cardputer.Display.setRotation(1);
    Serial.begin(115200);

    app_logic.SetCurrentState(&intro_screen_state);
}

void loop(void)
{
  M5Cardputer.update(); // needed for keyboard detection

  app_logic.Execute();
  tone_logic.Execute();


  M5Cardputer.Speaker.tone(440, 100);
  M5.delay(50);
}

//----------------------------------------------------------------

// for ESP-IDF compat
#if !defined ( ARDUINO ) && defined ( ESP_PLATFORM )
extern "C" {
  void loopTask(void*)
  {
    setup();
    for (;;) {
      loop();
    }
    vTaskDelete(NULL);
  }

  void app_main()
  {
    xTaskCreatePinnedToCore(loopTask, "loopTask", 8192, NULL, 1, NULL, 1);
  }
}
#endif
