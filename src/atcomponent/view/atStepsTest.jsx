
import Taro, { Component } from '@tarojs/taro'
import { Text, View } from '@tarojs/components'
import { AtSteps } from 'taro-ui'

import 'taro-ui/dist/style/index.scss'

//import './AtStepsTest.scss';

export default class AtStepsTest extends Component {
    constructor() {
        super(...arguments)
        this.state = { current: 0 }
    }

    onChange(current) {
        this.setState({ current })
    }

    render() {
        const items = [
            { 'title': 'step1', 'desc': 'step1 readme' },
            { 'title': 'step2', 'desc': 'step2 readme' },
            { 'title': 'step3', 'desc': 'step3 readme' }
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

