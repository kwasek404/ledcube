// Struktura danych przechowująca stan kostki LED
// cube[x][y][z], gdzie x, y, z przyjmują wartości od 0 do 2.
// Wartość 0 oznacza ZGASZONĄ diodę, 1 oznacza ZAPALONĄ diodę.
int cube[3][3][3];

// Definicje pinów
// Piny osi Z (wybór warstwy): Pin 10 (dla z=0), Pin 11 (dla z=1), Pin 12 (dla z=2)
const int layer_select_pins[3] = {10, 11, 12};

// Funkcja pomocnicza do uzyskania numeru pinu XY dla współrzędnych (x,y)
int get_xy_pin(int x, int y) {
  return (y * 3) + x + 1;
}

// Czas wyświetlania każdej warstwy w milisekundach (dla efektu POV)
const int layer_refresh_delay = 2;

// Zmienne do obsługi animacji
unsigned long last_animation_time = 0;
int animation_interval = 200; // Czas w ms między klatkami animacji deszczu (mniejsza wartość = szybszy deszcz)
int rain_density = 4; // Mniejsza wartość = gęstszy deszcz (np. 3 oznacza 1/3 szansy na nową kroplę)


void setup() {
  // Inicjalizacja wszystkich 12 pinów sterujących jako WYJŚCIA
  for (int i = 1; i <= 12; i++) {
    pinMode(i, OUTPUT);
  }

  // Ustawienie początkowego stanu pinów (zgodnie z zasadą "LOW = wyłączone"):
  for (int x_coord = 0; x_coord < 3; x_coord++) {
    for (int y_coord = 0; y_coord < 3; y_coord++) {
      digitalWrite(get_xy_pin(x_coord, y_coord), LOW);
    }
  }
  for (int i = 0; i < 3; i++) {
    digitalWrite(layer_select_pins[i], LOW);
  }

  // Inicjalizacja generatora liczb losowych (opcjonalnie, ale zalecane)
  // Jeśli pin A0 nie jest podłączony, może generować podobne sekwencje po każdym resecie.
  // Można go podłączyć do "pływającego" pinu analogowego lub innego źródła szumu.
  randomSeed(analogRead(A0));


  // Inicjalizacja struktury danych kostki (wszystkie diody zgaszone)
  for (int x_idx = 0; x_idx < 3; x_idx++) {
    for (int y_idx = 0; y_idx < 3; y_idx++) {
      for (int z_idx = 0; z_idx < 3; z_idx++) {
        cube[x_idx][y_idx][z_idx] = 0;
      }
    }
  }
}

// Funkcja wyświetlająca stan kostki LED (multipleksowanie)
void display() {
  for (int z_current_layer = 0; z_current_layer < 3; z_current_layer++) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(layer_select_pins[i], LOW);
    }
    for (int x_coord = 0; x_coord < 3; x_coord++) {
      for (int y_coord = 0; y_coord < 3; y_coord++) {
        digitalWrite(get_xy_pin(x_coord, y_coord), LOW);
      }
    }
    for (int y_coord = 0; y_coord < 3; y_coord++) {
      for (int x_coord = 0; x_coord < 3; x_coord++) {
        if (cube[x_coord][y_coord][z_current_layer] == 1) {
          digitalWrite(get_xy_pin(x_coord, y_coord), HIGH);
        }
      }
    }
    digitalWrite(layer_select_pins[z_current_layer], HIGH);
    delay(layer_refresh_delay);
  }
}

void animate_rain() {
  // Przesuń istniejące krople deszczu o jedną warstwę w dół
  for (int x = 0; x < 3; x++) {
    for (int y = 0; y < 3; y++) {
      // Warstwa dolna (z=0) przejmuje stan warstwy środkowej (z=1)
      cube[x][y][0] = cube[x][y][1];
      // Warstwa środkowa (z=1) przejmuje stan warstwy górnej (z=2)
      cube[x][y][1] = cube[x][y][2];
    }
  }

  // Wygeneruj nowe krople na górnej warstwie (z=2)
  for (int x = 0; x < 3; x++) {
    for (int y = 0; y < 3; y++) {
      // Losowo zdecyduj, czy w danym miejscu (x,y) na górnej warstwie pojawi się nowa kropla
      // random(rain_density) da liczbę od 0 do rain_density-1.
      // Jeśli wynikiem jest 0, tworzymy kroplę (szansa 1/rain_density).
      if (random(rain_density) == 0) {
        cube[x][y][2] = 1;
      } else {
        cube[x][y][2] = 0;
      }
    }
  }
}

void loop() {
  // Ciągłe wywoływanie funkcji display() w celu odświeżania obrazu na kostce LED.
  display();

  // Logika animacji deszczu
  if (millis() - last_animation_time > animation_interval) {
    last_animation_time = millis(); // Zapisz czas ostatniej aktualizacji animacji
    animate_rain(); // Wykonaj krok animacji deszczu
  }
}