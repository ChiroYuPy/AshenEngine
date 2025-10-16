#ifndef ASHEN_EDITORACTION_H
#define ASHEN_EDITORACTION_H

#include <memory>
#include <stack>

#include "Ashen/Scene/Node.h"

namespace ash {
    class EditorActions {
    public:
        virtual ~EditorActions() = default;
        virtual void Execute() = 0;
        virtual void Undo() = 0;
        [[nodiscard]] virtual std::string GetName() const = 0;
    };

    class TransformAction final : public EditorActions {
    public:
        TransformAction(Node* node, const Transform& oldT, const Transform& newT)
            : m_Node(node), m_OldTransform(oldT), m_NewTransform(newT) {}

        void Execute() override { m_Node->SetLocalTransform(m_NewTransform); }
        void Undo() override { m_Node->SetLocalTransform(m_OldTransform); }
        [[nodiscard]] std::string GetName() const override { return "Transform"; }

    private:
        Node* m_Node;
        Transform m_OldTransform, m_NewTransform;
    };

    class EditorActionStack {
    public:
        void Push(std::unique_ptr<EditorActions> action) {
            action->Execute();
            m_UndoStack.push(std::move(action));
            while (!m_RedoStack.empty()) m_RedoStack.pop();
        }

        void Undo() {
            if (m_UndoStack.empty()) return;
            auto action = std::move(m_UndoStack.top());
            m_UndoStack.pop();
            action->Undo();
            m_RedoStack.push(std::move(action));
        }

        void Redo() {
            if (m_RedoStack.empty()) return;
            auto action = std::move(m_RedoStack.top());
            m_RedoStack.pop();
            action->Execute();
            m_UndoStack.push(std::move(action));
        }

        [[nodiscard]] bool CanUndo() const { return !m_UndoStack.empty(); }
        [[nodiscard]] bool CanRedo() const { return !m_RedoStack.empty(); }

    private:
        std::stack<std::unique_ptr<EditorActions>> m_UndoStack, m_RedoStack;
    };
}

#endif //ASHEN_EDITORACTION_H