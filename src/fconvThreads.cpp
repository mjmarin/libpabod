// Customized by mjmarin

#include <fconvThreads.h>
#include <crossplatform.h>

#ifdef USE_PTHREADS
CvMat** fconv (CvMatND* features, CvMatND** filters, int a, int filtersDim)
{
  // Get A operand
  const CvMatND *mxA = features;  

  if (mxA->dims != 3)
    cout << "Invalid input: A" << endl;

  // Get B operand
  int num_bs = filtersDim;  

  // Get start and end
  int start = a - 1;
  int end = filtersDim - 1;

  if (start < 0 || end >= num_bs || start > end)
  cout << "Invalid input: start/end" << endl;

  int len = end - start + 1;

  // Output CvMat array
  assert (len > 0);
  CvMat **result = new CvMat* [len];
  assert (result != NULL);

  // Do convolutions
  thread_data *td = new thread_data [len];
  pthread_t *ts = new pthread_t [len];
  int ADims[3];
  getDimensions(mxA, ADims);
  double *A = getMatNData <double> (mxA);

  CvMatND *mxB;
  int height;
  int width;

  for (int i = 0; i < len; i++) 
  {
    mxB = filters[i+start];

    for (int d = 0; d < 3; d++)    
      td[i].ADims[d] = ADims[d];

    td[i].A = A;
    getDimensions(mxB, td[i].BDims);
    td[i].B = getMatNData <double> (mxB);

    if (mxB->dims != 3 || td[i].ADims[2] != td[i].BDims[2])
    cout << "Invalid input: B" << endl;

    // Output size is computed
    height = td[i].ADims[0] - td[i].BDims[0] + 1;
    width = td[i].ADims[1] - td[i].BDims[1] + 1;

    if (height < 1 || width < 1)
      cout << "Invalid input: B should be smaller than A" << endl;

    td[i].CDims[0] = height;
    assert (td[i].CDims[0] > 0);
    td[i].CDims[1] = width;
    assert (td[i].CDims[1] > 0);

    createMatrix (2, td[i].CDims, CV_64FC1, &(td[i].mxC));
    assert (td[i].mxC != NULL);
    td[i].C = new double [td[i].mxC->rows * td[i].mxC->cols];
    getMatData <double> (td[i].mxC, td[i].C);

    if (pthread_create (&ts[i], NULL, process, (void*) &td[i]))
      cout << "Error creating threads" << endl;
  }

  // Wait for the treads to finish and set return values
  void *status;

  for (int i = 0; i < len; i++) 
  {
    pthread_join(ts[i], &status);

    setMatData (td[i].mxC, td[i].C);
    result[i] = td[i].mxC;

    delete[] td[i].B;
    delete[] td[i].C;
  }

  delete[] A;
  delete[] td;
  delete[] ts;

  return result;  
}

#else

CvMat** fconv (CvMatND* features, CvMatND** filters, int a, int filtersDim)
{

  // Get A operand
  const CvMatND *mxA = features;  

  if (mxA->dims != 3)
    cout << "Invalid input: A" << endl;

  // Get B operand
  int num_bs = filtersDim;  

  // Get start and end
  int start = a - 1;
  int end = filtersDim - 1;

  if (start < 0 || end >= num_bs || start > end)
  cout << "Invalid input: start/end" << endl;

  int len = end - start + 1;

  // Output CvMat array
  assert (len > 0);
  CvMat **result = new CvMat* [len];
  assert (result != NULL);

  // Do convolutions
  thread_data *td = new thread_data [len];
//  pthread_t *ts = new pthread_t [len];
  int ADims[3];
  getDimensions(mxA, ADims);
  double *A = getMatNData <double> (mxA);

#ifdef USE_OPENMP
 
  int i;
  std::vector<CvMatND *> mxB2(len);
  omp_set_num_threads(len);
  #pragma omp parallel for
  for (i = 0; i < len; i++) 
  {
    //cout << "Thread " << omp_get_thread_num() << " of Num OMP-threads: " << omp_get_num_threads() << endl;
    mxB2[i] = filters[i+start];

    for (int d = 0; d < 3; d++)    
      td[i].ADims[d] = ADims[d];

    td[i].A = A;
    getDimensions(mxB2[i], td[i].BDims);
    td[i].B = getMatNData <double> (mxB2[i]);

//    if (mxB->dims != 3 || td[i].ADims[2] != td[i].BDims[2])
//       cout << "Invalid input: B" << endl;

    // Output size is computed
    //height = td[i].ADims[0] - td[i].BDims[0] + 1;
    //width = td[i].ADims[1] - td[i].BDims[1] + 1;

//    if (height < 1 || width < 1)
//      cout << "Invalid input: B should be smaller than A" << endl;

    td[i].CDims[0] = td[i].ADims[0] - td[i].BDims[0] + 1; // height;
    assert (td[i].CDims[0] > 0);
    td[i].CDims[1] = td[i].ADims[1] - td[i].BDims[1] + 1; // width;
    assert (td[i].CDims[1] > 0);

    createMatrix (2, td[i].CDims, CV_64FC1, &(td[i].mxC));
    assert (td[i].mxC != NULL);
    td[i].C = new double [td[i].mxC->rows * td[i].mxC->cols];
    getMatData <double> (td[i].mxC, td[i].C);

    process((void*) &td[i]);

    // Prepare data for output
    setMatData (td[i].mxC, td[i].C);
    result[i] = td[i].mxC;

    delete[] td[i].B;
    delete[] td[i].C;
  }

#else
  CvMatND *mxB;
  int height;
  int width;


  for (int i = 0; i < len; i++) 
  {    
    mxB = filters[i+start];

    for (int d = 0; d < 3; d++)    
      td[i].ADims[d] = ADims[d];

    td[i].A = A;
    getDimensions(mxB, td[i].BDims);
    td[i].B = getMatNData <double> (mxB);

    if (mxB->dims != 3 || td[i].ADims[2] != td[i].BDims[2])
    cout << "Invalid input: B" << endl;

    // Output size is computed
    height = td[i].ADims[0] - td[i].BDims[0] + 1;
    width = td[i].ADims[1] - td[i].BDims[1] + 1;

    if (height < 1 || width < 1)
      cout << "Invalid input: B should be smaller than A" << endl;

    td[i].CDims[0] = height;
    assert (td[i].CDims[0] > 0);
    td[i].CDims[1] = width;
    assert (td[i].CDims[1] > 0);

    createMatrix (2, td[i].CDims, CV_64FC1, &(td[i].mxC));
    assert (td[i].mxC != NULL);
    td[i].C = new double [td[i].mxC->rows * td[i].mxC->cols];
    getMatData <double> (td[i].mxC, td[i].C);

    process((void*) &td[i]);

    // Prepare data for output
    setMatData (td[i].mxC, td[i].C);
    result[i] = td[i].mxC;

    delete[] td[i].B;
    delete[] td[i].C;
  }


#endif

  delete[] A;
  delete[] td;
//  delete[] ts;


  return result;  
}

#endif


void* process (void *thread_arg) 
{
  thread_data *args = (thread_data *)thread_arg;
  double *A = args->A;
  double *B = args->B;
  double *C = args->C;
  const int *ADims = args->ADims;
  const int *BDims = args->BDims;
  const int *CDims = args->CDims;
  int num_features = args->ADims[2];

  double *dst;
  double *A_src;
  double *B_src;
  double val = 0;
  double *A_off;
  double *B_off;

  for (int f = 0; f < num_features; f++) 
  {
    dst = C;
    A_src = A + f*ADims[0]*ADims[1];    
    B_src = B + f*BDims[0]*BDims[1];

    for (int x = 0; x < CDims[1]; x++) 
    {
      for (int y = 0; y < CDims[0]; y++) 
      {
        val = 0;

        for (int xp = 0; xp < BDims[1]; xp++) 
        {
          A_off = A_src + (x+xp)*ADims[0] + y;
          B_off = B_src + xp*BDims[0];

          switch (BDims[0]) 
          {
            case 20: val += A_off[19] * B_off[19];
            case 19: val += A_off[18] * B_off[18];
            case 18: val += A_off[17] * B_off[17];
            case 17: val += A_off[16] * B_off[16];
            case 16: val += A_off[15] * B_off[15];
            case 15: val += A_off[14] * B_off[14];
            case 14: val += A_off[13] * B_off[13];
            case 13: val += A_off[12] * B_off[12];
            case 12: val += A_off[11] * B_off[11];
            case 11: val += A_off[10] * B_off[10];
            case 10: val += A_off[9] * B_off[9];
            case 9: val += A_off[8] * B_off[8];
            case 8: val += A_off[7] * B_off[7];
            case 7: val += A_off[6] * B_off[6];
            case 6: val += A_off[5] * B_off[5];
            case 5: val += A_off[4] * B_off[4];
            case 4: val += A_off[3] * B_off[3];
            case 3: val += A_off[2] * B_off[2];
            case 2: val += A_off[1] * B_off[1];
            case 1: val += A_off[0] * B_off[0];
              break;
            default:        
              for (int yp = 0; yp < BDims[0]; yp++) 
              val += *(A_off++) * *(B_off++);
          }

        }

        *(dst++) += val;
      }
    }
  }

#ifdef USE_PTHREADS
  pthread_exit (NULL);
//#else
//  return NULL;
#endif
  return NULL;
}




