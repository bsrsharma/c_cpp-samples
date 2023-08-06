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
#include "/eigen/Eigen/Core"
#include "/eigen/Eigen/LU"
#include <algorithm>
#include <array>

namespace tofcore {

using namespace Eigen;    

class CartesianTransform
{
public:

    CartesianTransform(
                      uint16_t imagerWidth, uint16_t imagerHeight, 
                      double rowOffset, double columnOffset, double rowFocalLength, double columnFocalLength,
                      const std::array<double, 5>& m_undistort_coeffs
    );

    ~CartesianTransform();
    void initLensTransform();
    void transformPixel(uint32_t srcX, uint32_t srcY, double srcZ, double &destX, double &destY, double &destZ);

private:

    typedef Eigen::Matrix<float_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> matrix_t;
    typedef Eigen::Map<matrix_t, Eigen::Unaligned> mapped_matrix_t;
    typedef Eigen::Matrix<float_t, 3, 3, Eigen::RowMajor> camera_matrix_t;
    typedef Eigen::Array<float_t, 5, 1> distortion_coeffs_t;
    typedef Eigen::Matrix<float_t, 3, 1> vector3_t;

   //These defaults are reasonable for the Mojave lens.
    float_t m_camera_matrix[9] { (float)247.041408, (float)0.0, (float)119.5, (float)0.0, (float)247.041408, (float)159.5, (float)0.0, (float)0.0, (float)1.0 };
    std::array <float_t, 5> m_undistort_coeffs = { (float)0.206391667, (float)0.131966667, (float)0.0, (float)0.0, (float)-0.0755416667 };

    matrix_t m_g_raysX;
    matrix_t m_g_raysZ;
    matrix_t m_g_raysY;

    uint16_t m_imagerWidth;
    uint16_t m_imagerHeight;
    
    /// @brief Apply new camera_matrix and undistortion coefficents, generating per pixel
    ///   ray data. 
    /// @param camera_matrix camera matrix describing lens offset and focal lengths. 
    /// @param lens_undistort_coeffs undistortion coefficents for the lens shape.
    void GenerateRays(Eigen::Ref<matrix_t> rays_x, Eigen::Ref<matrix_t> rays_y, Eigen::Ref<matrix_t> rays_z );

    void UndistortPoint(const vector3_t &pt, vector3_t &pt_undistort );

};

} //end namespace tofcore
/** @} */
#endif // __TOFCORE_CARTESIAN_TRANSFORM_H__
