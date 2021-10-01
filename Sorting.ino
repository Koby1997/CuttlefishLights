void selectionSort(int delayTime)
{
    for(int i = 0; i < NUM_LEDS - 1; i++)
    {
      int switchIndex = i;
      for(int j = i + 1; j < NUM_LEDS; j++)
      {
        if(colors[j] < colors[switchIndex])
          switchIndex = j;
      }
      int temp = colors[switchIndex];
      colors[switchIndex] = colors[i];
      colors[i] = temp;
      leds[i] = CHSV(colors[i], 255,255);
      leds[switchIndex] = CHSV(colors[switchIndex],255,255);
      FastLED.show();
      delay(delayTime);
    }
}


// void mergeSort(int front, int back)
// {
//     int mid = 0;

//     if(front < back)
//     {
//         mid = (front + back) / 2;

//         mergeSort(front, mid);
//         mergeSort(mid+1, back);

//         merge(front, mid, back);
//     }
// }

// void merge(int front, int back, int mid)
// {
//     int size = back - front + 1;
//     int mergedNumbers[size];
//     int mergePos = 0;
//     int leftPos = front;
//     int rightPos = mid + 1;

//     while(leftPos <= mid && rightPos <= back)
//     {
//         if(colors[leftPos] < colors[rightPos])
//             mergedNumbers[mergePos++] = colors[leftPos++];
//         else
//             mergedNumbers[mergePos++] = colors[rightPos++];      
//     }

//     while(leftPos <= mid)
//         mergedNumbers[mergePos++] = colors[leftPos++];

//     while(rightPos <= back)
//         mergedNumbers[mergePos++] = colors[rightPos++];

//     for(mergePos = 0; mergePos < size; mergePos++)
//     {
//         colors[front + mergePos] = mergedNumbers[mergePos];
//         leds[front + mergePos] = CHSV(colors[front + mergePos], 255,255);
//         FastLED.show();
//     }
// }

void bubbleSort(int delayTime)
{
    for(int i = 0; i < NUM_LEDS - 1; i++)
    {
        for(int j = 0; j < NUM_LEDS - 2; j++)
        {
            if(colors[j] > colors[j+1])
            {
                int tmp = colors[j+1];
                colors[j+1] = colors[j];
                leds[j+1] = CHSV(colors[j+1], 255, 255);
                colors[j] = tmp;
                leds[j] = CHSV(colors[j], 255, 255);
                FastLED.show();
                delay(delayTime);
            }
        }
    }
}


void insertionSort(int delayTime)
{
    for(int i = 0; i < NUM_LEDS -1; i ++)
    {
        for(int j = i + 1; j > 0; j--)
        {
            if(colors[j -1] > colors[j])
            {
                int tmp = colors[j-1];
                colors[j-1] = colors[j];
                colors[j] = tmp;
                
                leds[j+1] = CHSV(colors[j+1], 255, 255);
                leds[j] = CHSV(colors[j], 255, 255);
                FastLED.show();
                delay(delayTime);
            }
        }
    }
}

//heap code got from here, then modified
//https://www.tutorialspoint.com/heap-sort-in-chash
void heapSort(int delayTime)
{
    for(int i = (NUM_LEDS/2) - 1; i >= 0; i--)
        heapify(NUM_LEDS, i);

    for(int j = NUM_LEDS - 1; j >= 0; j--)
    {
        int tmp = colors[0];
        colors[0] = colors[j];
        colors[j] = tmp;

        leds[0] = CHSV(colors[0], 255, 255);
        leds[j] = CHSV(colors[j], 255, 255);
        FastLED.show();

        heapify(j,0);
    }
}

void heapify(int n, int i)
{
     int largest = i;
     int left = 2*i + 1;
     int right = 2*i + 2;
     if (left < n && colors[left] > colors[largest])
        largest = left;
     if (right < n && colors[right] > colors[largest])
        largest = right;
     if (largest != i)
     {
        int swap = colors[i];
        colors[i] = colors[largest];
        colors[largest] = swap;

        leds[i] = CHSV(colors[i], 255, 255);
        leds[largest] = CHSV(colors[largest], 255, 255);
        FastLED.show();
        
        heapify(n, largest);
    }
}


//broken
//
// //quickSort got from here, then modified
// //https://www.tutorialspoint.com/chash-program-to-perform-quick-sort-using-recursion
// void quickSort(int left, int right, int delayTime)
// {
//     int pivot;
//    if (left < right)
//    {
//         pivot = partition(left, right, delayTime);
//         Serial.println("PIVOT = ");
//         Serial.print(pivot);
//         Serial.println("Left = ");
//         Serial.print(left);
//         Serial.println("Right");
//         Serial.print(right);
//       if (pivot > 1)
//          quickSort(left, pivot - 1, delayTime);

//       if (pivot + 1 < right)
//          quickSort(pivot + 1, right, delayTime);
//    }
// }

// int partition(int left, int right, int delayTime)
// {
//    int pivot = colors[left];
//    while(true)
//    {Serial.println(8);
//       while (colors[left] < pivot)
//       {
//         left++;
//         Serial.print(1);
//       }

//       while (colors[right] > pivot)
//       {
//          right--;
//          Serial.print(2);
//       }

//       if (left < right)
//       {
//          Serial.print(9);
//          if(colors[left] == colors[right])
//             return right;
          
//          int temp = colors[right];
//          colors[right] = colors[left];
//          colors[left] = temp;

//          leds[right] = CHSV(colors[right], 255, 255);
//          leds[left] = CHSV(colors[left], 255, 255);
//          FastLED.show();
//          delay(delayTime);
//       }
//       else
//       {
//          Serial.print(3);
//             break;
//       }
//    }
//    Serial.print(4);
//    return right;
// }
