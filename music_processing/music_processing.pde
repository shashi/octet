import ddf.minim.analysis.*;
import ddf.minim.*;
import processing.serial.*;

Minim minim;  
AudioPlayer track;
FFT fft;
Serial port;

int X_AXIS=0, Y_AXIS=1, Z_AXIS=2, WINDOW_SIZE = 10;

boolean[] found_beat = new boolean[10];
float[] last_beat;

float[] lastSpectrum, spectralFlux, fluxAverages, thresholdFunction;
float[] beats;
int[] beat_times = new int[10];
float threshold_gravity = 0.02;
float peak_max=0;
int[] partitions = new int[10];

float MULTIPLIER = 6;
int parts = 3;
int NUM_GRAPHS = 3;

boolean rubber_band=false;
int startX, startY;
int last_sent = 0; 

void mouseReleased() {
  rubber_band = true;
  startX = mouseX;
  startY = mouseY;
}

void setup()
{
  size(512, 600, P3D);

  partitions[0] = 10;
  partitions[1] = 20;
  partitions[2] = 30;

  minim = new Minim(this);
  track = minim.loadFile("Deliverance.mp3");

  fft = new FFT(track.bufferSize(), track.sampleRate());
  fft.window(fft.HAMMING);
  fft.logAverages(22, 3);

  lastSpectrum = new float[fft.avgSize()];
  spectralFlux = new float[fft.avgSize()];
  thresholdFunction = new float[fft.avgSize()];
  beats = new float[fft.avgSize()];
  last_beat = new float[fft.avgSize()];
  fluxAverages = new float[10];
  track.play();

  String portName = Serial.list()[0];
  println(Serial.list());
  port = new Serial(this, portName, 9600);
  //myPort.write(c);
  rectMode(CORNERS);
}

float w, x0, y0, edge, avg_i, diff;

void draw()
{
  background(0);
  noStroke();
  fill(255);

  // Find forward fft of the track
  fft.forward(track.mix);

  // scaling factor
  float shift = 5.0 / NUM_GRAPHS;
  
  // width of a histo line
  w = int(width/fft.avgSize());
  
  // Calculate spectralFlux 
  for(int i = 0; i < fft.avgSize(); i++)
  {
    avg_i = fft.getAvg(i);

    diff = avg_i - lastSpectrum[i];
    spectralFlux[i] = diff < 0 ? 0 : diff;

    rect(i * w, height / NUM_GRAPHS, i * w + w, height / NUM_GRAPHS - shift * fft.getAvg(i));
    rect(i * w, height, i * w + w, height - shift * spectralFlux[i]);

    lastSpectrum[i] = avg_i;
  }

  int[] peaks = new int[10];
  int begin, end, peak_at = 0;
  float peak=0, decay;
  // calculate thresholdFunction
  for (int i=0; i < fft.avgSize(); i++) {
    begin = Math.max(0, i - WINDOW_SIZE / 2);
    end = Math.min(fft.avgSize(), i + WINDOW_SIZE / 2);

    thresholdFunction[i] = 0; 
    for (int j=begin; j < end; j++) {
      thresholdFunction[j] += spectralFlux[j];
    }
    thresholdFunction[i] /= (end - begin);
    thresholdFunction[i] *= MULTIPLIER;
    //thresholdFunction[i] *= thresholdFunction[i];
    

    if (thresholdFunction[i] < spectralFlux[i]) {
      beats[i] = spectralFlux[i] - thresholdFunction[i];
    } else {
      beats[i] = 0;
    }
    // plot beat
    //rect(i * w, 2 * height / NUM_GRAPHS, i * w + w, 2 * height / NUM_GRAPHS - shift * beats[i]);

    // XXX: grr just one peak?
    if (peak < beats[i]) {
      peak = beats[i];
      peak_at = i;
    }
  }

  if (peak > 10) {
    last_beat[peak_at] = millis();
  }
  
  float k;
  // bouncy beat boxes thing
  for (int i=0; i < fft.avgSize(); i++) {
    decay = pow(2.87, -0.02 * (millis() - last_beat[i]));

    k = 100;
    edge = k * decay;

    x0 = i*w + (w - edge) / 2;
    y0 = 300 + (100 - edge) / 2;
    rect(x0, y0, x0 + edge, y0 + edge);

  }
  
/*
  // find beats by gravity method
  int begin = 0;
  float decay = 0;

  for(int i=0; i<parts; i++) {
    fluxAverages[i] = 0;
    for(int j=begin; j < partitions[i]; j++) {
      fluxAverages[i] += spectralFlux[j];  
    }

    fluxAverages[i] /= (partitions[i] - begin);
    fluxAverages[i] *= 1.5;
    begin = partitions[i];

    if (fluxAverages[i] > beats[i]) {
      beats[i] = fluxAverages[i];
      beat_times[i] = 0; 
    } else {
      // mutate beat and make it fall
      beat_times[i]++;
      beats[i] -= 0.5 * threshold_gravity * (2 * beat_times[i] - 1);
    }
    
    found_beat[i] =(fluxAverages[i] > beats[i] * 0.75);
    if (found_beat[i]) {
      since_last_beat[i] = 0;
    } else {
      since_last_beat[i]++;
    }
    
    decay = pow(2.87, -0.1 * since_last_beat[i]);

    w = width / parts;
    edge = w * decay;

    x0 = i*w + (w - edge) / 2;
    y0 = 400 + (w-edge) / 2;
    rect(x0, y0, x0 + edge, y0 + edge);

    // Send signals to the cube

    if (i == 0 && millis() - last_sent > 50) {
      int count = (int) round(decay * 4);
      //if (boolean(port.available())) {
        /*
        int count = 7 - round(decay * 8);
        if (count >= 0) {
          set_plane(Y_AXIS, count, 3);
        } else {
          set_plane(Y_AXIS, count, 0);
        } 

        //wireframe_cube(4-count, 4 -  count, 4 - count, 2*count, 3);
      //}
    }
  }
  */
}

// Cube functions

void fill_cube(char val) {
  String send = "";
  send += '0';
  send += (char) val;
  send += '.';
  port.write(send);
}

void random_set_plane(int axis, int plane, int count, int level) {
  String send = "";
  send += '4';
  send += (char) axis;
  send += (char) plane;
  send += (char) count;
  send += (char) level;
  send += '.';
  port.write(send);
}

void set_plane(int axis, int plane, int level) {
  String send = "";
  /*
  send += '1';
  send += (char) axis;
  send += (char) plane;
  send += (char) level;
  send += '.';
*/
  port.write(send);
}

void shift_cube(int axis) {
  port.write("5" + (char) axis + ".");
}

void wireframe_cube(int x, int y, int z, int edge, int level) {
  String send = "";
  send += '3';
  send += (char) x;
  send += (char) y;
  send += (char) z;
  send += (char) edge;
  send += (char) level;
  send += '.';

  port.write(send);
}

void stop()
{
  // always close Minim audio classes when you are done with them
  track.close();
  // always stop Minim before exiting
  minim.stop();
  super.stop();
}
