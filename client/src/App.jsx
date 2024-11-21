import React, { useState, useEffect } from 'react';
import { graphviz } from 'd3-graphviz';
import { Select, Input, Button } from 'antd';
import './App.css';

function App() {
    const [kind, setKind] = useState('')
    const [input, setInput] = useState('')
    const [data, setData] = useState(null)

    const handleSubmit = async () => {
        try {
            const res = await fetch(`http://localhost:8080/${kind}`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ input: input })
            })
            if (!res.ok) {
                throw new Error('Network response was not ok')
            }
            const result = await res.json()
            setData(result)
            console.log('Data fetched:', result)
        } catch (err) {
            console.log('Error fetching data:', err)
        }
    }

    const generateGraph = (data, kind) => {
        kind = kind.toUpperCase()
        let dotString = `digraph ${kind} {\n`

        dotString += 'rankdir=LR;\n'

        data.states.forEach((state) => {
            if (data.accs.includes(state.id)) {
                dotString += `${state.id} [shape=doublecircle];\n`
            } else {
                dotString += `${state.id} [shape=circle];\n`
            }

            if (data.inits.includes(state.id)) {
                dotString += `${state.id} [style=bold,color=blue];\n`
            }

            state.edges?.forEach((edge) => {
                dotString += `${state.id} -> ${edge.target} [label="${edge.character}"];\n`
            })
        })

        dotString += ' }'
        return dotString
    }


    const renderGraph = () => {
        if (data && kind) {
            const dotString = generateGraph(data, kind)
            try {
                graphviz('#graph_container')
                    .renderDot(dotString)
                    .zoom(false)
                    .fit(true)
            } catch (error) {
                console.error('Error rendering graph:', error)
            }
        }
    }

    useEffect(() => {
        renderGraph()
    }, [data, kind])

    return (
        <div>
            <h1>正则表达式转换有限状态机</h1>
            <Input
                type='text'
                value={input}
                onChange={(e) => setInput(e.target.value)}
                placeholder='注意输入格式（*表示闭包，|表示选择）'
            />
            <div style={{ marginTop: '20px' }}>
                <Select
                    placeholder='选择状态机类型'
                    options={
                        [
                            { value: 'nfa', label: 'NFA' },
                            { value: 'dfa', label: 'DFA' },
                            { value: 'mdfa', label: 'MDFA' }
                        ]
                    }
                    size='big'
                    onChange={(value) => setKind(value)}
                    style={{ marginRight: '20px' }}
                />

                <Button
                    type='primary'
                    size='middle'
                    disabled={!(input && kind)}
                    onClick={handleSubmit}
                >
                    生成状态机
                </Button>
            </div>
            <div id='graph_container' />
        </div >
    )
}

export default App