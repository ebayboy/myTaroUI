/* Steps 步骤条 */
/* TODO: 'success':true, 'error': true 没生效 */

import Taro, { Component } from '@tarojs/taro'
import { Text, View } from '@tarojs/components'
import { AtSteps } from 'taro-ui'

import './atStepsTest.scss';

export default class AtStepsTest extends Component {
    constructor() {
        super(...arguments)
        this.state = { current: 1 }
    }

    onChange(current) {
        this.setState({ current })
    }

    render() {
        const items = [
            { 'title': 'step1', 'desc': 'step1 readme', 'success': true },
            {
                'title': 'step2',
                'desc': 'custom icon',
                'icon': {
                    value: 'camera',
                    activeColor: '#fff',
                    inactiveColor: '#78A4FA',
                    size: '14',
                }
            },
            { 'title': 'step3', 'desc': 'step3 readme', 'error': true }
        ]

        return (
            < View >
                <Text>AtStepsTest</Text>
                <AtSteps items={items}
                    current={this.state.current}
                    onChange={this.onChange.bind(this)}
                >
                </AtSteps>
            </View>
        )
    }
}

