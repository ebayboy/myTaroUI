import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtInputNumber } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtInputNumberTest extends Component {
    constructor() {
        super(...arguments)

        this.state = { value: 0 }
    }

    handleChange(value) {
        console.log('change', value)
        this.setState({ value: value })
        return value
    }

    render() {
        return (
            <View>
                <Text>AtInputNumberTest</Text>
                <AtInputNumber
                    type='digit'
                    value={this.state.value}
                    min={0}
                    max={10}
                    step={0.1}
                    onChange={this.handleChange.bind(this
                    )}>
                </AtInputNumber>
            </View>
        )
    }
}