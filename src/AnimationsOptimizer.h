#ifndef ANIMATIONSOPTIMIZER_H
#define ANIMATIONSOPTIMIZER_H

#include "pch.h"
#include "QLogger/QLogger.h"

/*!
 * \brief The AnimationsOptimizer class will handle all operations related to animations (hkx files)
 */

class AnimationsOptimizer : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(AnimationsOptimizer)

public:
    /*!
     * \brief Port an Oldrim animation to Skyrim Special Edition using Bethesda Havok Post Processing Tool
     * \param filePath The path of the file to optimize
     */
    static void optimize(const QString& filePath);
};

#endif // ANIMATIONSOPTIMIZER_H
