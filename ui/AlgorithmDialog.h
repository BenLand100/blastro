#pragma once
#include "core/WorkspaceRegistry.h"
#include <QWidget>
#include <map>
#include <string>

namespace blastro {

class AlgorithmDialog : public QWidget {
    Q_OBJECT
public:
    AlgorithmDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~AlgorithmDialog() override = default;

    virtual std::map<std::string, std::string> getConfig() const = 0;
    virtual std::string algorithmName() const = 0;
    virtual void refreshWorkspaceElements() {}

signals:
    void algorithmExecuted(const std::string& name, const std::map<std::string, std::string>& config);

protected:
    WorkspaceRegistry& m_workspace;
};

} // namespace blastro
