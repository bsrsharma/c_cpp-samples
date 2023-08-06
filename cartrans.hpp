#ifndef __TOFCORE_CARTESIAN_TRANSFORM_H__
#define __TOFCORE_CARTESIAN_TRANSFORM_H__
/**
 * @defgroup cartesian_transformation Cartesian Transformation
 *
 * @brief Cartesian Transformation
 *
 * @file cartesian_transformation.hpp
 *
 * Manages lens information and calculations related to lens characteristics. 
 *
 * @{
 */
#include "/eigen/Eigen/CORE"
#include "/eigen/Eigen/LU"
#include <vector>

#include <iostream>

namespace tofcore {

    using namespace Eigen;

    class CartesianTransform
    {
        typedef Eigen::Matrix<float_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> matrix_t;
        typedef Eigen::Map<matrix_t, Eigen::Unaligned> mapped_matrix_t;
        typedef Eigen::Matrix<float_t, 3, 3, Eigen::RowMajor> camera_matrix_t;
        typedef Eigen::Array<float_t, 5, 1> distortion_coeffs_t;
        typedef Eigen::Matrix<float_t, 3, 1> vector3_t;

    public:
        ///Imager width in pixels
        uint32_t IMAGER_WIDTH = 320;

        ///Imager height in pixels
        uint32_t IMAGER_HEIGHT = 240;

        //These defaults are reasonable for the Mojave lens.
        float_t camera_matrix[9]{ 247.041408, 0.0, 119.5, 0.0, 247.041408, 159.5, 0.0, 0.0, 1.0 };
        float_t undistort_coeffs[5]{ 0.206391667, 0.131966667, 0.0, 0.0, -0.0755416667 };

        CartesianTransform();
        ~CartesianTransform();
        void initLensTransform();
        void transformPixel(uint32_t srcX, uint32_t srcY, double srcZ, double& destX, double& destY, double& destZ);

    private:

        matrix_t g_raysX;
        matrix_t g_raysZ;
        matrix_t g_raysY;

        int32_t numCols;
        int32_t numRows;

        /// @brief Apply new camera_matrix and undistortion coefficents, generating per pixel
        ///   ray data. 
        /// @param camera_matrix camera matrix describing lens offset and focal lengths. 
        /// @param undistort_coeffs undistortion coefficents for the lens shape.             

        void GenerateRays(Eigen::Ref<matrix_t> rays_x, Eigen::Ref<matrix_t> rays_y, Eigen::Ref<matrix_t> rays_z,
            const camera_matrix_t& camera_matrix, const distortion_coeffs_t& lens_undistort_coeffs);


        void UndistortPoint(const vector3_t& pt, vector3_t& pt_undistort, const distortion_coeffs_t& lens_undistort_coeffs);

    };


} //end namespace tofcore
/** @} */
#endif // __TOFCORE_CARTESIAN_TRANSFORM_H__