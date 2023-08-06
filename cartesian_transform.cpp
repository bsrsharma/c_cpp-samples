/*
 * @defgroup cartesian_transformation Cartesian Transformation
 *
 * @brief Cartesian Transformation
 * 
 * @file cartesian_transformation.cpp
 *
 * 
 * Manages lens information and calculations related to lens characteristics.
 *
 */
#include "cartesian_transform.hpp"
#include <iostream>

namespace tofcore{

using namespace Eigen;    

CartesianTransform::CartesianTransform(
                                       uint16_t imagerWidth, uint16_t imagerHeight, 
                                       double rowOffset, double columnOffset, double rowFocalLength, double columnFocalLength,
                                       const std::array<double, 5>& undistort_coeffs
                                      )
{

   m_imagerWidth = imagerWidth;
   m_imagerHeight = imagerHeight;

   m_camera_matrix[0] = rowOffset;
   m_camera_matrix[1] = 0.0;
   m_camera_matrix[2] = columnOffset;
   m_camera_matrix[3] = 0.0;
   m_camera_matrix[4] = rowFocalLength;
   m_camera_matrix[5] = columnFocalLength;
   m_camera_matrix[6] = 0.0;
   m_camera_matrix[7] = 0.0;
   m_camera_matrix[8] = 1.0;

   std::copy(undistort_coeffs.begin(), undistort_coeffs.end(), m_undistort_coeffs.begin());

   m_g_raysX.resize(imagerHeight, imagerWidth);
   m_g_raysY.resize(imagerHeight, imagerWidth);
   m_g_raysZ.resize(imagerHeight, imagerWidth);
}

CartesianTransform::~CartesianTransform(){

}

void CartesianTransform::initLensTransform()
{
    GenerateRays(m_g_raysX, m_g_raysY, m_g_raysZ);
}


// function for cartesian transfrmation
void CartesianTransform::transformPixel(uint32_t srcX, uint32_t srcY, double srcZ, double &destX, double &destY, double &destZ)
{

    assert(srcX < m_imagerWidth);
    assert(srcY < m_imagerHeight);

    if ((srcX >= m_imagerWidth) || (srcY >= m_imagerHeight))
    {
        destX = -1.0;
        destY = -1.0;
        destZ = -1.0;

        return;
    }

    destX = srcZ * m_g_raysX(srcY,srcX);
    destY = srcZ * m_g_raysY(srcY,srcX);
    destZ = srcZ * m_g_raysZ(srcY,srcX);
}

/// @brief Generate unit vectors x,y,z rays for all pixels based on the lens coefficents.
/// @param rays_x matrix to place the x term for each ray.
/// @param rays_y matrix to place the y term for each ray.
/// @param rays_z matrix to place the z term for each ray.
/// @param camera_matrix camera matrix for terms for the lens.
/// @param lens_undistort_coeffs undistortion coefficients for the lens.
void CartesianTransform::GenerateRays(Eigen::Ref<matrix_t> rays_x, Eigen::Ref<matrix_t> rays_y, Eigen::Ref<matrix_t> rays_z )
{
    // Get size of image
    const auto m = rays_x.rows();
    const auto n = rays_x.cols();

    // invert the camera matrix so we can go from homogeneous coordinates -> 3D
    camera_matrix_t camera_mat = (camera_matrix_t)m_camera_matrix;
std::cerr  << "camera_mat \n" << camera_mat << "\n";    
    auto inv_camera_mat = camera_mat.inverse();
std::cerr << "inv_camera_mat \n" << inv_camera_mat << "\n";
    // loop through pixels and compute each ray
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            // compute ray for (i,j) 
            vector3_t u(i, j, 1.0);
            vector3_t x = inv_camera_mat * u;

            // apply lens undistortion to point
            vector3_t x_undistort;
            UndistortPoint(x, x_undistort /*, lens_undistort_coeffs */ );

            // normalize ray to unit length
            // NOTE: Entries are permuted to go from homogeneous coordinates to 3D RHS coordinates
            // (x up/down, y right/left, z forward) -> (x forward, y left/right, z up/down)
            auto xnorm = x_undistort.norm();
            rays_x(i, j) = x_undistort(2) / xnorm;
            rays_y(i, j) = -x_undistort(1) / xnorm;
            rays_z(i, j) = -x_undistort(0) / xnorm;

        }
    }
}

/// @brief Use lens undistortion parameters to correct ray vector for pixel.
/// @param pt 
/// @param pt_undistort 
/// @param lens_undistort_coeffs 
void CartesianTransform::UndistortPoint(const vector3_t &pt, vector3_t &pt_undistort)
{
    // Modify x (by reference) according to lens distortion parameters
    // ASSUMES x(2)==1!
    const auto &k1 = m_undistort_coeffs[0];
    const auto &k2 = m_undistort_coeffs[1];
    const auto &p1 = m_undistort_coeffs[2];
    const auto &p2 = m_undistort_coeffs[3];
    const auto &k3 = m_undistort_coeffs[4];

    float_t x_sq = 0., y_sq = 0., xy = 0., r_sq = 0., r4 = 0., r6 = 0.;
    float_t gamma = 0.;

    // === calculate gamma
    x_sq = pt(0) * pt(0);
    y_sq = pt(1) * pt(1);
    xy = pt(0) * pt(1);
    r_sq = x_sq + y_sq;
    r4 = r_sq * r_sq;
    r6 = r_sq * r4;
    gamma = 1 + k1 * r_sq + k2 * r4 + k3 * r6;

    // === calculate undistorted point
    pt_undistort(0) = pt(0) * gamma + 2 * p1 * xy + p2 * (r_sq + 2 * x_sq);
    pt_undistort(1) = pt(1) * gamma + p1 * (r_sq + 2 * y_sq) + 2 * p2 * xy;
    pt_undistort(2) = 1.0;
 
}

} //end namespace tofcore

int main()
{
    const std::array<double, 5> undis = { 0.206391667, 0.131966667, 0.0, 0.0, -0.0755416667 };
    static tofcore::CartesianTransform cart(320, 240,
                                            (float)247.041408, (float)119.5, (float)247.041408, (float)159.5,
                                            undis
                                            );

    cart.initLensTransform();

    uint32_t srcX;
    uint32_t srcY;
    double srcZ;
    double destX;
    double destY;
    double destZ;

    double expected_destX[] = { 0.0, 12989.9, 7478.18, 2514.67, 15040.4, 9585.09, 4067.25, 17787.8, 12811.7, 7324.73 };
    double expected_destY[] = { 0.0, -5660.21, -1512.21, 86.6557, 4118.61, 4955.95, -2603.28, -6447.84, -1612.7, 766.91 };
    double expected_destZ[] = { 0.0, 355.473, -3390.54, 198.798, -5970.39, 1336.67, -1470.62, 3391.64, -3569.09, 1759.38 };

    std::cerr << "srcX\tsrcY\tsrcZ\tdestX\t\tdestY\t\t\tdestZ\n\n";
    for (uint32_t i = 0; i < 10; i++) {
        srcX = (i * 314159) % 319;
        srcY = (i * 314159) % 239;
        srcZ = (double)((i * 314159) % 19999);
        destX = -9999.0;
        destY = -9999.0;
        destZ = -9999.0;

        cart.transformPixel(srcX, srcY, srcZ, destX, destY, destZ);

        std::cerr << srcX << "\t" << srcY << "\t" << srcZ << "\t" << destX << "\t\t" << destY << "\t\t\t" << destZ << "\n";
    }
}
