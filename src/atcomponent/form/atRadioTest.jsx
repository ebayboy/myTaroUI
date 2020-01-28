import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtForm, AtRadio } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtRadioTest extends Component {
    constructor() {
        super(...arguments)
        this.state = {
            value: '',
            options: [
                { label: '单选项一', value: 'option1', desc: '单选项描述' },
                { label: '单选项二', value: 'option2' },
                { label: '单选项三禁用', value: 'option3', desc: '单选项描述', disabled: true }
            ]
        }
    }

    handleClick(value) {
        this.setState({ value: value })
        console.log("state:", this.state.value)
    }

    render() {
        return (
            <View>
                <AtRadio
                    value={this.state.value}
                    options={this.state.options}
                    onClick={this.handleClick.bind(this)}
                ></AtRadio>
            </View>
        )
    }
}