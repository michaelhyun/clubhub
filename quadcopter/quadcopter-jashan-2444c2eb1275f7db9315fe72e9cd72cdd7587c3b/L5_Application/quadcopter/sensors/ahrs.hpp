/*
 * ahrs.hpp
 *
 *  Created on: Aug 2, 2014
 *      Author: pardeep
 */

#ifndef AHRS_HPP_
#define AHRS_HPP_


void AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz, int);
void getYawPitchRoll(float *ypr);


#endif /* AHRS_HPP_ */
