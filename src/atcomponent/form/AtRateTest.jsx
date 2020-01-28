import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtForm, AtRate } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtRateTest extends Component {
    constructor() {
        super(...arguments)

        this.state = { value: 2 }
    }

    handleChange(value) {
        console.log("value:", value)
        this.setState({ value: value })
    }

    render() {
        return (
            <View>
                <AtRate
                    value={this.state.value}
                    onChange={this.handleChange.bind(this)}
                />
            </View>
        )
    }
}