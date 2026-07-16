// Huanyu,miaomiaomiao,nekoneko


#include "AuraAbilityTypes.h"

bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	uint32 RepBits = 0;
	// 判断当前是否在保存数据
	if (Ar.IsSaving())
	{
		//保存数据时，如果有对应的配置项数据，那么将对应位的值设置为1
		if (bReplicateInstigator && Instigator.IsValid())
		{
			RepBits |= 1 << 0;
		}
		if (bReplicateEffectCauser && EffectCauser.IsValid() )
		{
			RepBits |= 1 << 1;
		}
		if (AbilityCDO.IsValid())
		{
			RepBits |= 1 << 2;
		}
		if (bReplicateSourceObject && SourceObject.IsValid())
		{
			RepBits |= 1 << 3;
		}
		if (Actors.Num() > 0)
		{
			RepBits |= 1 << 4;
		}
		if (HitResult.IsValid())
		{
			RepBits |= 1 << 5;
		}
		if (bHasWorldOrigin)
		{
			RepBits |= 1 << 6;
		}
		// 自定义内容，增加暴击和格挡触发存储
		if (bIsBlockedHit)
		{
			RepBits |= 1 << 7;
		}
		if (bIsCriticalHit)
		{
			RepBits |= 1 << 8;
		}
		if(bIsSuccessfulDeBuff)
		{
			RepBits |= 1 << 9;
		}
		if(DeBuffDamage > 0.f)
		{
			RepBits |= 1 << 10;
		}
		if(DeBuffDuration > 0.f)
		{
			RepBits |= 1 << 11;
		}
		if(DeBuffFrequency > 0.f)
		{
			RepBits |= 1 << 12;
		}
		if(DamageType.IsValid())
		{
			RepBits |= 1 << 13;
		}
		if(!DeathImpulse.IsZero())
		{
			RepBits |= 1 << 14;
		}
		if (!KnockbackForce.IsZero())
		{
			RepBits |= 1 << 15;
		}
		if (bIsRadialDamage)
		{
			RepBits |= 1 << 16;

			if (RadialDamageInnerRadius > 0.f)
			{
				RepBits |= 1 << 17;
			}
			if (RadialDamageOuterRadius > 0.f)
			{
				RepBits |= 1 << 18;
			}
			if (!RadialDamageOrigin.IsZero())
			{
				RepBits |= 1 << 19;
			}
		}
		
	}
	
	// 序列化RepBits
	// 使用了多少长度，就将长度设置为多少
	Ar.SerializeBits(&RepBits, 19);
	
	if (RepBits & (1 << 0))
	{
		Ar << Instigator;
	}
	if (RepBits & (1 << 1))
	{
		Ar << EffectCauser;
	}
	if (RepBits & (1 << 2))
	{
		Ar << AbilityCDO;
	}
	if (RepBits & (1 << 3))
	{
		Ar << SourceObject;
	}
	if (RepBits & (1 << 4))
	{
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	}
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = MakeShared<FHitResult>();
			}
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}
	// 新增对暴击格挡的序列化或反序列化处理
	if (RepBits & (1 << 7))
	{
		Ar << bIsBlockedHit;
	}
	if (RepBits & (1 << 8))
	{
		Ar << bIsCriticalHit;
	}
	// 修改后的布尔值序列化部分
	// if (RepBits & (1 << 7)) 
	// {
	// 	Ar.SerializeBits(&bIsBlockedHit, 1);
	// }
	// if (RepBits & (1 << 8)) 
	// {
	// 	Ar.SerializeBits(&bIsCriticalHit, 1);
	// }

	if (RepBits & (1 << 9))
	{
		Ar << bIsSuccessfulDeBuff;
	}
	if (RepBits & (1 << 10))
	{
		Ar << DeBuffDamage;
	}
	if (RepBits & (1 << 11))
	{
		Ar << DeBuffDuration;
	}
	if (RepBits & (1 << 12))
	{
		Ar << DeBuffFrequency;
	}
	if (RepBits & (1 << 13))
	{
		if (Ar.IsLoading()) //判断是否在加载资源
		{
			if (!DamageType.IsValid())
			{
				DamageType = TSharedPtr<FGameplayTag>(new FGameplayTag());
			}
		}
		DamageType->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 14))
	{
		DeathImpulse.NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 15))
	{
		KnockbackForce.NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 16))
	{
		Ar << bIsRadialDamage;
		
		if (RepBits & (1 << 17))
		{
			Ar << RadialDamageInnerRadius;
		}
		if (RepBits & (1 << 18))
		{
			Ar << RadialDamageOuterRadius;
		}
		if (RepBits & (1 << 19))
		{
			RadialDamageOrigin.NetSerialize(Ar, Map, bOutSuccess);
		}
	}
	// 如果是加载数据（即反序列化）时，需要调用对ASC进行初始化
	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // 只是为了初始化 InstigatorAbilitySystemComponent
	}
	bOutSuccess = true;
	return true;
}
