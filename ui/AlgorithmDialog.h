#pragma once
#include "core/WorkspaceRegistry.h"
#include <QDialog>
#include <map>
#include <string>

namespace blastro {

class AlgorithmDialog : public QDialog {
    Q_OBJECT
public:
    AlgorithmDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~AlgorithmDialog() override = default;

    virtual std::map<std::string, std::string> getConfig() const = 0;
    virtual std::string algorithmName() const = 0;

signals:
    void algorithmExecuted(const std::string& name, const std::map<std::string, std::string>& config);

protected:
    WorkspaceRegistry& m_workspace;
};

} // namespace blastro
