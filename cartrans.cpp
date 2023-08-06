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
#include "cartrans.hpp"
#include <iostream>

namespace tofcore {

    using namespace Eigen;

    CartesianTransform::CartesianTransform() {
        g_raysX.resize(IMAGER_HEIGHT, IMAGER_WIDTH);
        g_raysZ.resize(IMAGER_HEIGHT, IMAGER_WIDTH);
        g_raysY.resize(IMAGER_HEIGHT, IMAGER_WIDTH);
    }

    CartesianTransform::~CartesianTransform() {

    }

    void CartesianTransform::initLensTransform()
    {
        numCols = IMAGER_WIDTH;
        numRows = IMAGER_HEIGHT;

        GenerateRays(g_raysX, g_raysY, g_raysZ, (camera_matrix_t)camera_matrix, (distortion_coeffs_t)undistort_coeffs);
    }


    // function for cartesian transfrmation
    void CartesianTransform::transformPixel(uint32_t srcX, uint32_t srcY, double srcZ, double& destX, double& destY, double& destZ)
    {
        assert(srcX < IMAGER_WIDTH);
        assert(srcY < IMAGER_HEIGHT);

        if ((srcX >= IMAGER_WIDTH) || (srcY >= IMAGER_HEIGHT))
        {
            destX = -1.0;
            destY = -1.0;
            destZ = -1.0;

            return;
        }

        destX = srcZ * g_raysX(srcY, srcX);
        destY = srcZ * g_raysY(srcY, srcX);
        destZ = srcZ * g_raysZ(srcY, srcX);
    }

    /// @brief Generate unit vectors x,y,z rays for all pixels based on the lens coefficents.
    /// @param rays_x matrix to place the x term for each ray.
    /// @param rays_y matrix to place the y term for each ray.
    /// @param rays_z matrix to place the z term for each ray.
    /// @param camera_matrix camera matrix for terms for the lens.
    /// @param lens_undistort_coeffs undistortion coefficients for the lens.
    void CartesianTransform::GenerateRays(Eigen::Ref<matrix_t> rays_x, Eigen::Ref<matrix_t> rays_y, Eigen::Ref<matrix_t> rays_z,
        const camera_matrix_t& camera_matrix, const distortion_coeffs_t& lens_undistort_coeffs)
    {
        // Get size of image
        const auto m = rays_x.rows();
        const auto n = rays_x.cols();
        std::cerr << "camera_matrix = \n" << camera_matrix; std::cerr << "\nlens_undistort_coeffs = \n" << lens_undistort_coeffs << "\n";
        // invert the camera matrix so we can go from homogeneous coordinates -> 3D
        auto inv_camera_mat = camera_matrix.inverse();

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
                UndistortPoint(x, x_undistort, lens_undistort_coeffs);

                // normalize ray to unit length
                // NOTE: Entries are permuted to go from homogeneous coordinates to 3D RHS coordinates
                // (x up/down, y right/left, z forward) -> (x forward, y left/right, z up/down)
                auto xnorm = x_undistort.norm();
                //std::cerr << "x_undistort(0) = " <<  x_undistort(0) << " xnorm = " << xnorm << "\n";
                rays_x(i, j) = x_undistort(2) / xnorm;
                rays_y(i, j) = -x_undistort(1) / xnorm;
                rays_z(i, j) = -x_undistort(0) / xnorm;

                if (i == j)
                std::cerr << "rays_x(" << i << "," << j << ") = " << rays_x(i,j) <<
                             " , rays_y(" << i << "," << j << ") = " << rays_y(i, j) <<
                             " , rays_z(" << i << "," << j << ") = " << rays_z(i, j) << "\n";
            }
        }
    }

    /// @brief Use lens undistortion parameters to correct ray vector for pixel.
    /// @param pt 
    /// @param pt_undistort 
    /// @param lens_undistort_coeffs 
    void CartesianTransform::UndistortPoint(const vector3_t& pt, vector3_t& pt_undistort, const distortion_coeffs_t& lens_undistort_coeffs)
    {
        // Modify x (by reference) according to lens distortion parameters
        // ASSUMES x(2)==1!
        const auto& k1 = lens_undistort_coeffs(0);
        const auto& k2 = lens_undistort_coeffs(1);
        const auto& p1 = lens_undistort_coeffs(2);
        const auto& p2 = lens_undistort_coeffs(3);
        const auto& k3 = lens_undistort_coeffs(4);

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
    static tofcore::CartesianTransform cart;

    cart.IMAGER_WIDTH = 320;
    cart.IMAGER_HEIGHT = 240;

    float my_cm[] = { 247.041408, 0.0, 119.5, 0.0, 247.041408, 159.5, 0.0, 0.0, 1.0 };
    float my_uc[] = { 0.206391667, 0.131966667, 0.0, 0.0, -0.0755416667 };
    memcpy(cart.camera_matrix, my_cm, 9);
    memcpy(cart.undistort_coeffs, my_uc, 5);

    cart.initLensTransform();

    srand(314159265);

    for (int i = 0; i < 10; i++) {
        uint32_t srcX = rand() % 320;
        uint32_t srcY = rand() % 240;
        double srcZ = (double)(rand() % 20000);
        double destX = -9999.0;
        double destY = -9999.0;
        double destZ = -9999.0;

        cart.transformPixel(srcX, srcY, srcZ, destX, destY, destZ);

        std::cerr << "srcX = " << srcX << ", srcY = " << srcY << ", srcZ = " << srcZ << " ; ";
        std::cerr << "destX = " << destX << ", destY = " << destY << ", destZ = " << destZ << "\n";
    }
}
